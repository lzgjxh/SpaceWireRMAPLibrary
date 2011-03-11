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

class Main {
private:
	
	int menu; //selected command number
	
	unsigned int address;
	unsigned int length; //in a unit of byte
	vector<unsigned char> writedata;
	vector<unsigned char>* readdata;
public:
	Main(){}
	~Main(){}
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
		//loop
		if(menu!=9){
			goto loop;
		}
	}

private:
	void showTitle() {
		cout << "############################################" << endl;
		cout << "###  SpaceWire RMAPPacket test program   ###" << endl;
		cout << "############################################" << endl << endl;
	}

	void showMenu() {
		cout << "Menu : " << endl;
		cout << "  Create an RMAP packet     [1]" << endl;
		cout << "  Interpret an RMAP packet  [2]" << endl;
		cout << "  Quit                      [9]" << endl << endl;
	}
private:
	void createpacket(){
		//??
	}
	void interpretpacket(){
		vector<unsigned char> data;
		data=commandLinePacketCreation();
		RMAPPacket packet;
		try{
			packet.setPacket(&data);
			packet.dumpPacket();
		}catch(RMAPException e){
			cout << "This is not an RMAP Packet" << endl;
			SpaceWireUtilities::dumpPacket(&data);
		}
		cout << endl;
	}
private:
	int getSelection() {
		string str;
		int value;
		select:
		cout << "(select) > ";
		cin >> str;
		value=atoi(str.c_str());
		if(0<value) {
			cout << endl;
			return value;
		}
		goto select;
	}

	void processCommand(int menu) {
		switch (menu) {
		case 1: //Receive a raw SpW packet
			createpacket();
			break;
		case 2: //Send a raw SpW packet
			interpretpacket();
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

};


int main(int argc,char* argv[]){
	Main main;
	main.run();
	return 0;
}