/** This program simulates RMAP target node which contains
 * 1MB RAM. Other nodes can read/write this memory via RMAP.
 * Memory Map:
 *  0x0000-0000 ~ 0x000F-FFFF  RAM
 * 
 * If an RMAP access exceeds the memory space of RAM, this
 * program will reply failure to the source node. 
 */

#include "SpaceWire.hh"
#include "RMAP.hh"

#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

class Main {
private:
	SpaceWireIF* spacewireif;
	SpaceWireCLI cli;
	unsigned char* ram;
	static const int ramsize=1024*1024;
public:
	Main(){}
public:
	void run(){
		//show title
		cout << "===============================================" << endl;
		cout << "           RMAP Target Node Simulator          " << endl;
		cout << "               version 2008-06-25              " << endl;
		cout << "" << endl;
		cout << " This program utilizes SpaceWire/RMAP Library. " << endl;
		cout << "===============================================" << endl;
		cout << "" << endl;
		
		//initialize and open SpW IF
		spacewireif=SpaceWireIFImplementations::selectInstanceFromCommandLineMenu();
		spacewireif->initialize();
		spacewireif->open();
		
		//initialize ram
		ram=(unsigned char*)malloc(ramsize); //1MB
		for(int i=0;i<ramsize;i++){
			ram[i]=0x00;
		}
		
		//set packet dump mode
		bool dumpmode=selectPacketDumpMode();
		
		//loop
		cout << "running..." << endl;
		while(true){
			if(dumpmode){
				cout << "waiting Command packet..." << endl;
			}
			
			vector<unsigned char> packet=spacewireif->receive();
			RMAPPacket commandpacket;
			RMAPPacket replypacket;
			try{
				//interpret received packet
				commandpacket.setPacket(&packet);
				
				//dump Command
				if(dumpmode){
					cout << "Command packet is..." << endl;
					commandpacket.dump();
				}
				
				//process Command and create Reply packet
				replypacket=processCommand(commandpacket);
				
				//dump Reply
				if(dumpmode){
					cout << "Reply packet is..." << endl;
					replypacket.dump();
				}
				
				//reply Reply packet
				spacewireif->send(replypacket.getPacket());
				
			}catch(RMAPException e){
				cout << "received packet is not an RMAP packet" << endl;
			}
		}
	}
	
	bool selectPacketDumpMode(){
		int choice;
		cout << "Dump Mode:" << endl;
		cout << "  on             [1]" << endl;
		cout << "  off            [2]" << endl;
		select_dump:
		cli.ask2_int(cin,choice,cout,"select > ");
		if(0<choice && choice<3) {
			switch(choice){
				case 1:
					return true;
					break;
				case 2:
					return false;
					break;
				default:
					break;
			}
		}
		goto select_dump;
	}
	
	RMAPPacket processCommand(RMAPPacket& commandpacket){
		RMAPPacket replypacket;
		unsigned int address=commandpacket.getAddress();
		unsigned int length=commandpacket.getLength();
		
		replypacket.setRMAPDestination(*(commandpacket.getRMAPDestination()));
		replypacket.setReplyPacket();
		replypacket.setWriteOrRead(commandpacket.getWriteOrRead());
		replypacket.setTransactionID(commandpacket.getTransactionID());
		replypacket.setVerifyMode(commandpacket.getVerifyMode());
		replypacket.setVerifyMode(commandpacket.getVerifyMode());
		replypacket.setAckMode(commandpacket.getAckMode());
		replypacket.setIncrementMode(commandpacket.getIncrementMode ());
		replypacket.setExtendedAddress(commandpacket.getExtendedAddress());
		replypacket.setAddress(address);
		replypacket.setLength(length);
		
		//perform Write/Read command with ordered address and length 
		if(commandpacket.isReadMode()){
			if(address<ramsize && (address+length)<ramsize){
				//set data
				vector<unsigned char> data;
				for(int i=0;i<length;i++){
					data.push_back(ram[address+i]);
				}
				replypacket.setData(&data);
				replypacket.setReplyStatus(RMAPPacket::CommandExcecutedSuccessfully); //successfull
			}else{
				//size or address error
				replypacket.setReplyStatus(RMAPPacket::GeneralError); //unsuccessfull
			}
		}
		
		if(commandpacket.isWriteMode()){
			if(address<ramsize && (address+length)<ramsize){
				for(int i=0;i<length;i++){
					ram[address+i]=commandpacket.getData()->at(i);
				}
				replypacket.setReplyStatus(RMAPPacket::CommandExcecutedSuccessfully); //successfull
			}else{
				//size or address error
				replypacket.setReplyStatus(RMAPPacket::GeneralError); //unsuccessfull
			}
		}
		
		replypacket.createPacket();
		return replypacket;
	}
};

int main(int argc,char* argv[]){
	Main main;
	main.run();
	return 0;
}

/** History
 * 2008-06-25 file created (Takayuki Yuasa)
 * 2008-09-04 use SpaceWireIFImplementations::selectInstanceFromCommandLineMenu() (Takayuki Yuasa)
 */
