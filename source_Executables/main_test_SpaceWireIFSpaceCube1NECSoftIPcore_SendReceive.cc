/** Sample program for sending/receiving packet via SpaceWireIFSpaceCube1NECSoftIPcore.
 */

#include "SpaceWire.hh"
#include "RMAP.hh"

#include "Condition.hh"
#include "Mutex.hh"
#include "Thread.hh"

#include <exception>
#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
using namespace std;

class SingleTransaction : public Thread {
private:
	unsigned int id;
	Mutex* mutex;
	RMAPEngine* rmapengine;
	RMAPSocket* rmapsocket;
	RMAPDestination rmapdestination;
	unsigned int address;
	unsigned int length;
	static int activethreads;
public:
	SingleTransaction(unsigned int newid,Mutex* newmutex,RMAPEngine* newrmapengine,RMAPDestination newrmapdestination,unsigned int newaddress,unsigned int newlength)
	: Thread(),id(newid),mutex(newmutex),rmapengine(newrmapengine),rmapdestination(newrmapdestination),address(newaddress),length(newlength){}
	~SingleTransaction(){}
	void countup(){
		mutex->lock();
		activethreads++;
		mutex->unlock();
	}
	static int getActiveThreads(){
		return activethreads;
	}
	void run(){
		rmapsocket=rmapengine->openRMAPSocketTo(rmapdestination);
		mutex->lock();
		cout << "Thread id:" << id << " Read (address=" << setw(8) << setfill('0') << hex << address;
		cout << " length=" << setw(2) << setfill('0') << length << ") activethreads=" << activethreads << endl; 
		mutex->unlock();
		vector<unsigned char>* data=rmapsocket->read(address,length);
		mutex->lock();
		cout << "Thread id:"<< id << " Completed (replied data length=" << setw(2) << setfill('0') << data->size() << ")" << endl;
		cout << "received data : ";
		SpaceWireUtilities::dumpPacket(data);
		activethreads--;
		mutex->unlock();
		rmapengine->closeRMAPSocket(rmapsocket);
		this->exit();
	}
};

int SingleTransaction::activethreads=0;

class SendReceive {
private:
	SpaceWireIF* spacewireif;
	RMAPEngine* rmapengine;
	
	SpaceWireCLI cli;
	
	int menu; //selected command number
	
	unsigned int address;
	unsigned int length; //in a unit of byte
	vector<unsigned char> writedata;
	vector<unsigned char>* readdata;
	
	map<unsigned int,RMAPPacket*> packetmap;
	static const unsigned int ExpectedPacketNumber=8;
	
	Mutex mutex;
public:
	SendReceive(){
		//initialize SpaceWire IF
		spacewireif=SpaceWireIFImplementations::selectInstanceFromCommandLineMenu();
		//open SpW IF
		spacewireif->initialize();
		spacewireif->open();
		//open RMAPEngine
		rmapengine=new RMAPEngine(spacewireif);
		rmapengine->start();
		rmapengine->suspend();
	}
	~SendReceive(){
		rmapengine->stop();
		spacewireif->close();
		delete rmapengine;
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
		cout << "  MT Test                   [3]" << endl;
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
		cout << "Multiple Transaction Test (Master Mode)" << endl;
		cout << "This program runs RMAPEngine to perform simultaneuos" << endl
			 << "multiple transactions via single SpaceWireIF." << endl;
		rmapengine->resume();
		SingleTransaction* st[ExpectedPacketNumber];
		
		for(unsigned int i=0;i<ExpectedPacketNumber;i++){
			RMAPDestination rmapdestination;
			rmapdestination.setDestinationKey(i);
			unsigned int address=i;
			unsigned int length=i+1;
			st[i]=new SingleTransaction(i,&mutex,rmapengine,rmapdestination,address,length);
			st[i]->countup();
			st[i]->start();
		}
		
		Condition condition;
		while(SingleTransaction::getActiveThreads()!=0){
			cout << "SingleTransaction::getActiveThreads() : " << SingleTransaction::getActiveThreads() << endl;
			condition.wait(1000);
		}
		cout << endl;
		cout << "Completed" << endl << endl;
		rmapengine->suspend();
	}
	void receiveSpWPacket(){
		cout << "Receive a raw SpW packet" << endl;
		cout << "waiting a packet...";
		vector<unsigned char> receiveddata;
		receiveddata=rmapengine->receive();
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
		rmapengine->send(&sentdata);
		cout << "done" << endl;
		cout << endl;
	}

	void setDestination(){
	}
};


int main(int argc,char* argv[]){
	SendReceive sr;
	sr.run();
	return 0;
}