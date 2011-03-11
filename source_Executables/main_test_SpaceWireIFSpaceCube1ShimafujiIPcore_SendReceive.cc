/** Sample program for sending/receiving packet via SpaceWireIFSpaceCube1ShimafujiIPcore.
 */

#include "RMAPPacket.hh"
#include "SpaceWireCLI.hh"
#include "SpaceWireIF.hh"
#include "SpaceWireIFSpaceCube1ShimafujiIPcore.hh"
#include "SpaceWireUtilities.hh"

#include <exception>
#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
using namespace std;

class SendReceive {
private:
	SpaceWireIF* spacewireif;

	SpaceWireCLI cli;

	int menu; //selected command number

	unsigned int address;
	unsigned int length; //in a unit of byte
	vector<unsigned char> writedata;
	vector<unsigned char>* readdata;

	map<unsigned int,RMAPPacket*> packetmap;
	static const unsigned int ExpectedPacketNumber=8;
public:
	SendReceive(int port){
		//initialize SpaceWire IF
		spacewireif=new SpaceWireIFSpaceCube1ShimafujiIPcore();
		//open SpW IF
		spacewireif->initialize();
		spacewireif->open(port);
	}
	~SendReceive(){
		spacewireif->close();
		delete spacewireif;
	}
	void run(){
		//title
		showTitle();

		loop:

		//show menu
		showMenu();
		//selection
		cli.ask2_int(cin,menu,cout,"select > ");
		//process selected menu
		processCommand(menu);
	}

private:
	void showTitle() {
		cout << "############################################" << endl;
		cout << "### SpaceWire sample program SendReceive ###" << endl;
		cout << "############################################" << endl << endl;
	}

	void showMenu() {
		cout << "Menu : " << endl;
		cout << "  Receive a raw SpW packet  [1]" << endl;
		cout << "  Send a raw SpW packet     [2]" << endl;
		cout << "  MT Test (Target)          [3]" << endl;
		cout << "  Quit                      [9]" << endl << endl;
	}
private:
	void processCommand(int menu) {
		switch (menu) {
		case 1: //Receive a raw SpW packet
			receiveSpWPacket();
			break;
		case 2: //Send a raw SpW packet
			sendSpWPacket();
			break;
		case 3:
			mtTest();
			break;
		case 9: //Quit
			break;
		}
	}
	unsigned int askAddress(){
		unsigned int value;
		cli.ask2_hex(cin,value,cout,"address in hex > ");
		return value;
	}
	unsigned int askLength(){
		int value;
		asklengthloop:
		cli.ask2_int(cin,value,cout,"length in decimal > ");
		if(value<0){
			goto asklengthloop;
		}else{
			return value;
		}
	}
	void askData(unsigned int ofAddress,vector<unsigned char>* datavector){
		unsigned int value;
		stringstream ss;
		askDataLoop:
		ss << "data for " << hex << setfill('0') << setw(4) << (unsigned int)(ofAddress/0x00010000);
		ss << "-" << hex << setfill('0') << setw(4) << (unsigned int)(ofAddress%0x00010000);
		ss << " > ";
		cli.ask2_hex(cin,value,cout,ss.str().c_str());
		if(value<0 || value>0x10000){
			cout << "out of range (0x0000-0xffff)" << endl;
			goto askDataLoop;
		}else{
			datavector->push_back(value%0x0100);
			datavector->push_back(value/0x0100);
		}
	}
	vector<unsigned char> commandLinePacketCreation(){
		cout << "enter packet data to be sent (in hex)..." << endl;
		cout << "to finish input, type a number greater than 0xFF" << endl;
		unsigned int data=0x00;
		vector<unsigned char> dummy;
		unsigned int choice;

		inputdummypacket:
			dummy.clear();
			data=0x00;
			while(0<=data && data<0x100){
				cout << setw(4) << setfill('0') << dummy.size() << " >";
				cin >> hex >> data;
				if(0<=data && data<0x100){
					dummy.push_back(data);
				}
			}
			cout << endl;
		confirmthenreturn:
			cout << "Entered Packet values are : " << endl;
			SpaceWireUtilities::dumpPacket(&dummy);
			cout << endl;
			cout << "Select : " << endl;
			cout << " 1 : OK" << endl;
			cout << " 2 : NG, re-input" << endl;
			cout << " 3 : Cancel Sending (return to menu)" << endl;
			cout << ">";
			cin >> choice;
			cout << endl;
			switch(choice){
			case 1:
				return dummy;
				break;
			case 2:
				goto inputdummypacket;
				break;
			case 3:
				dummy.clear();
				return dummy; //size zero
				break;
			default:
				goto confirmthenreturn;
				break;
			}
	}

private:
	void mtTest(){
		cout << "Multiple Transaction Test (Target Mode)" << endl;
		cout << "This program imitates multiple SpaceWire nodes." << endl;
		unsigned int packetcount=0;
		while(packetcount!=ExpectedPacketNumber){
			cout << "Waiting a packet (" << dec << packetcount << "/" << dec << ExpectedPacketNumber << "received)...";
			vector<unsigned char> data=spacewireif->receive();
			//SpaceWireUtilities::dumpPacket(&data);
			if(data.size()!=0){
				RMAPPacket* packet=new RMAPPacket();
				try{
					packet->setPacket(&data);
					unsigned int tid=packet->getTransactionID();
					cout << "received (TID=" << hex << tid << ")" << endl;
					packetmap.insert(pair<unsigned int,RMAPPacket*>(tid,packet));
					packetcount++;
				}catch(RMAPException e){
					cout << "RMAPException : not an RMAP packet" << endl;
					SpaceWireUtilities::dumpPacket(&data);
				}catch(exception e){
					cout << "exception : not an RMAP packet" << endl;
					SpaceWireUtilities::dumpPacket(&data);
				}
			}
		}
		cout << endl;
		cout << "Start Replying" << endl;
		vector<unsigned int> repliedtid;
		while(packetmap.size()!=0){
			cout << "Remaining TIDs : ";
			map<unsigned int,RMAPPacket*>::iterator i=packetmap.begin();
			while(i!=packetmap.end()){
				cout << i->first << " ";
				i++;
			}
			cout << endl;
			int tid;
			tidloop:
			cli.ask2_int(cin,tid,cout,"select tid to be replied> ");
			if(packetmap.find(tid)!=packetmap.end()){
				RMAPPacket* pointer=(packetmap.find(tid))->second;
				RMAPPacket commandpacket=*(pointer);
				delete pointer;
				commandpacket.interpretPacket();
				RMAPPacket* replypacket=new RMAPPacket();
				RMAPDestination destination;
				destination.setDestinationKey(commandpacket.getRMAPDestination()->getDestinationKey());
				replypacket->setDestination(destination);

				replypacket->setReplyStatus(0x00); //successfull
				replypacket->setReplyPacket();
				replypacket->setWriteOrRead(commandpacket.getWriteOrRead());
				replypacket->setTransactionID(commandpacket.getTransactionID());
				replypacket->setVerifyMode(commandpacket.getVerifyMode());
				replypacket->setVerifyMode(commandpacket.getVerifyMode());
				replypacket->setAckMode(commandpacket.getAckMode());
				replypacket->setIncrementMode(commandpacket.getIncrementMode ());
				replypacket->setExtendedAddress(commandpacket.getExtendedAddress());
				replypacket->setAddress(commandpacket.getAddress());
				replypacket->setLength(commandpacket.getLength());
				if(commandpacket.isReadMode()){
					vector<unsigned char> data;
					for(int i=0;i<commandpacket.getLength();i++){
						data.push_back((unsigned char)(i%0x100));
					}
					replypacket->setData(&data);
				}
				replypacket->createPacket();
				cout << "replying...";
				spacewireif->send(replypacket->getPacket());
				cout << "done" << endl << endl;
				delete replypacket;
				packetmap.erase(tid);
			}else{
				goto tidloop;
			}
		}
		cout << "Reply Completed" << endl << endl;
	}
	void receiveSpWPacket(){
		cout << "Receive a raw SpW packet" << endl;
		cout << "waiting a packet...";
		vector<unsigned char> receiveddata;
		spacewireif->setDebugMode();
		try{
			receiveddata=spacewireif->receive();
		}catch(SpaceWireException e){
			e.dump();
			exit(-1);
		}
		cout << "received" << endl << endl;
		cout << "Raw packet dump" << endl;
		SpaceWireUtilities::dumpPacket(&receiveddata);
		cout << endl;
		string choice;
		choiceLoop:
		cli.ask2(cin,choice,cout,"Interpret this packet as an RMAP packet? (y or n) : ");
		if(choice=="y" || choice=="Y"){
			try{
				RMAPPacket p;
				p.setPacket(&receiveddata);
				p.dumpPacket();
			}catch(RMAPException e){
				cout << "This seems not to be a proper RMAP packet." << endl << endl;
			}
		}else if(choice=="n" || choice=="N"){

		}else{
			goto choiceLoop;
		}
	}
	void sendSpWPacket(){
		cout << "Send a raw SpW packet" << endl;
		vector<unsigned char> sentdata;
		sentdata=commandLinePacketCreation();
		cout << "Sending...";
		spacewireif->send(&sentdata);
		cout << "done" << endl;
		cout << endl;
	}

	void setDestination(){
	}
};


int main(int argc,char* argv[]){
	int port=1;
	if(argc==2){
		port=atoi(argv[1]);
	}
	SendReceive sr(port);
	sr.run();
	return 0;
}
