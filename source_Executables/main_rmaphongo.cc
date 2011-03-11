#include "SpaceWire.hh"
#include "RMAP.hh"

#include <iostream>
#include <iomanip>
using namespace std;

//#define DEBUG

/** Command line RMAP controller "RMAPHongo".
 */
class RMAPHongo {
private:
	SpaceWireIF* spacewireif;
	RMAPEngine* rmapengine;
	RMAPDestination* rmapdestination;
	RMAPSocket* rmapsocket;

	SpaceWireCLI cli;

	int menu; //selected command number

	unsigned int address;
	unsigned int length; //in a unit of byte
	vector<unsigned char> writedata;
	vector<unsigned char>* readdata;

public:
	RMAPHongo(){
		//initialize SpaceWire IF
		spacewireif=SpaceWireIFImplementations::selectInstanceFromCommandLineMenu();
		spacewireif->initialize();
		spacewireif->open();

		//create an instance of RMAPEngine
		rmapengine=new RMAPEngine(spacewireif);
		rmapengine->start();

		//create an instance of RMAPDestination
		rmapdestination=new RMAPDestination();
		rmapdestination->setDestinationLogicalAddress(0x30);
		rmapdestination->setDestinationKey(0x02);

		//open RMAPSocket to "rmapdestination"
		rmapsocket=rmapengine->openRMAPSocketTo(*rmapdestination);
	}
	~RMAPHongo(){
		rmapengine->closeRMAPSocket(rmapsocket);
		rmapengine->stop();
		spacewireif->close();
		delete rmapengine;
		delete spacewireif;
	}
	void run(){
		//title
		showTitle();

		//set destination information
		cout << "Please set Destination information" << endl;
		setDestination();

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
		cout << "### SpaceWire sample program : rmaphongo ###" << endl;
		cout << "###           ver 2.1 (20080624)         ###" << endl;
		cout << "############################################" << endl << endl;
	}
	void setDestination() {
		rmapdestination=RMAPDestinationKnownDestinations::selectInstanceFromCommandLineMenu();
		reopenSocket();
	}
	void showMenu() {
		cout << "Menu : " << endl;
		cout << "  Read(2bytes)              [1]" << endl;
		cout << "  Write(2bytes)             [2]" << endl;
		cout << "  Read(general)             [3]" << endl;
		cout << "  Write(general)            [4]" << endl;
		cout << "  Receive a raw SpW packet  [5]" << endl;
		cout << "  Send a raw SpW packet     [6]" << endl;
		cout << "  Set Destination Info      [8]" << endl;
		cout << "  TimeCode Send            [10]" << endl;
		cout << "  TimeCode Get             [11]" << endl;
		cout << "  Toggle Debug Mode [" << (spacewireif->isDebugMode()? "On]    ":"Off]   ") << "[0]" << endl;
		cout << "  Quit                      [9]" << endl << endl;


	}
private:
	void processCommand(int menu) {
		writedata.clear();
		switch (menu) {
		case 1: //Read 2bytes
			read2bytes();
			break;
		case 2: //Write 2bytes
			write2bytes();
			break;
		case 3: //Read (general)
			readGeneral();
			break;
		case 4: //Write (general)
			writeGeneral();
			break;
		case 5: //Receive a raw SpW packet
			receiveSpWPacket();
			break;
		case 6: //Send a raw SpW packet
			sendSpWPacket();
			break;
		case 8: //Set Destination Info
			setDestination();
			break;
		case 10:
			sendTimeCode();
			break;
		case 11:
			getTimeCode();
			break;
		case 0:
			spacewireif->toggleDebugMode();
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
	int askData(unsigned int ofAddress,vector<unsigned char>* datavector){
		unsigned int value;
		stringstream ss;
		ss << "data for " << hex << setfill('0') << setw(4) << (unsigned int)(ofAddress/0x00010000);
		ss << "-" << hex << setfill('0') << setw(4) << (unsigned int)(ofAddress%0x00010000);
		ss << " > ";
		cli.ask2_hex(cin,value,cout,ss.str().c_str());
		datavector->push_back(value%0x0100);
		datavector->push_back(value/0x0100);
		return value;
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
	void read2bytes(){
		cout << "Read (2bytes)" << endl;
		address=askAddress();
		cout << "Reading...";
		try{
			readdata=rmapsocket->read(address,2);
		}catch(RMAPException e){
			SpaceWireUtilities::dumpPacket(rmapsocket->getTheLastReplyPacket().getPacket());
		}
		cout << "done" << endl;
		cout << "Read result : " << endl;
		//SpaceWireUtilities::printVectorWithAddress(address,readdata);
		cout << setfill('0') << hex << setw(4) << address/0x00010000 << "-" << setfill('0') << hex << setw(4) << address%0x00010000 << " " << setfill('0') << hex << setw(4) << (readdata->at(1))*0x0100+(readdata->at(0)) << endl;
		cout << endl;
	}
	void write2bytes(){
		cout << "Write (2bytes)" << endl;
		address=askAddress();
		writedata.clear();
		askData(address,&writedata);
		cout << "Writing...";
		rmapsocket->write(address,&writedata);
		cout << "done" << endl << endl;
	}
	void readGeneral(){
		cout << "Read (general)" << endl;
		address=askAddress();
		length=askLength();
		cout << "Reading...";
		readdata=rmapsocket->read(address,length);
		cout << "done" << endl;
		cout << "Read result : " << endl;
		SpaceWireUtilities::printVectorWithAddress2bytes(address,readdata);
		cout << endl;
	}
	void writeGeneral(){
		cout << "Write (general)" << endl;
		cout << "enter packet data to be sent (in hex)..." << endl;
		cout << "to finish input, type a number greater than 0xFFFF" << endl;
		address=askAddress();
		writedata.clear();
		int i=0;
		int value;
		do{
			value=askData(address+i,&writedata);
			i=i+2;
		}while(value<0x10000);
		writedata.pop_back(); //erase End of data separator(>0xFF)
		writedata.pop_back();
		cout << "Writing...";
		rmapsocket->write(address,&writedata);
		cout << "done" << endl << endl;
	}
	void receiveSpWPacket(){
		cout << "Receive a raw SpW packet" << endl;
		cout << "waiting a packet...";

		rmapengine->suspend();
		vector<unsigned char> receiveddata=rmapengine->receive();
		cout << "received" << endl << endl;
		cout << "Raw packet dump" << endl;
		SpaceWireUtilities::dumpPacket(&receiveddata);
		cout << endl;
		string choice;
		choiceLoop:
		cli.ask2(cin,choice,cout,"Interpret this packet as an RMAP packet? (y or n) : ");
		if(choice=="y" || choice=="Y"){
			try{
				RMAPPacket packet;
				packet.setPacket(&receiveddata);
				packet.dumpPacket();
			}catch(RMAPException e){
				cout << "This seems not to be a proper RMAP packet." << endl << endl;
			}
		}else if(choice=="n" || choice=="N"){

		}else{
			goto choiceLoop;
		}
		rmapengine->resume();
	}
	void sendSpWPacket(){
		cout << "Send a raw SpW packet" << endl;
		vector<unsigned char> sentdata;
		sentdata=commandLinePacketCreation();
		rmapengine->suspend();
		cout << "Sending...";
		rmapengine->send(&sentdata);
		cout << "done" << endl;
		rmapengine->resume();
		cout << endl;
	}
	void dumpCurrentRMAPDestination(){
		rmapdestination->dump();
	}
	void reopenSocket(){
		rmapengine->closeRMAPSocket(rmapsocket);
		rmapsocket=rmapengine->openRMAPSocketTo(*rmapdestination);
	}
	void sendTimeCode(){
		cout << "Input TimeCode to be sent" << endl;
		cout << "dec > ";
		int flag_and_timecode;
		cin >> dec >> flag_and_timecode;
		cout << endl;
		cout << "Sending TimeCode...";
		try{
			spacewireif->sendTimeCode((unsigned char)(flag_and_timecode));
		}catch(SpaceWireException e){
			e.dump();
		}
		cout << "done" << endl << endl;
	}
	void getTimeCode(){
		cout << "Getting TimeCode" << endl;
		cout << "TimeCode is 0x" << setw(2) << setfill('0') << hex << (unsigned int)spacewireif->getTimeCode() << endl << endl;
	}
};

int main(int argc,char** argv) {
	RMAPHongo rmaphongo;
	rmaphongo.run();
}

/** History
 * 2008-04-18 file create (Takayuki Yuasa)
 * 2008-05-01 added RMAPEngine (Takayuki Yuasa)
 * 2008-05-11 ver 2.0 (Takayuki Yuasa)
 * 2008-06-24 Takayuki Yuasa, aaded SpaceWireIFViaSpW2TCPIPBridge for POSIX implementation
 * 2008-06-24 ver 2.1 (Takayuki Yuasa)
 * 2008-09-01 MHI SpaceWire Universal IO Board support added (Takayuki Yuasa)
 * 2008-09-04 use SpaceWireIFImplementations::selectInstanceFromCommandLineMenu() (Takayuki Yuasa)
 * 2008-12-17 added TimeCode-related functions (Takayuki Yuasa)
*/
