#include "SpaceWire.hh"
#include "RMAP.hh"

#include <iostream>
using namespace std;

class Main {
public:
	Main(){
		//Initialization
	}
public:
	void run(){
		SpaceWireIF* spacewireif=SpaceWireIFImplementations::selectInstanceFromCommandLineMenu();
		spacewireif->initialize();
		spacewireif->open();
				
		//create an instance of RMAPEngine
		RMAPEngine* rmapengine=new RMAPEngine(spacewireif);
		rmapengine->start();
		
		//create an instance of RMAPDestination for SpaceWire Router Board by NEC
		RMAPDestination router_configurationregister_rmapdestination;
		router_configurationregister_rmapdestination.setDestinationLogicalAddress(0xFE);
		router_configurationregister_rmapdestination.setDestinationKey(0x00);
		router_configurationregister_rmapdestination.setWordWidth(4); //4bytes-1word
		vector<unsigned char> destinationpathaddress;
		destinationpathaddress.push_back(0x00);
			//router's internal "configuration register" is connected to port 0
		router_configurationregister_rmapdestination.setDestinationPathAddress(destinationpathaddress);
		
		//open RMAPSocket to "rmapdestination"
		RMAPSocket* rmapsocket=rmapengine->openRMAPSocketTo(router_configurationregister_rmapdestination);
		
		//RMAP Read to SpaceWireRouter Board's internal registers
		unsigned int addressOfLogicaladdressregister=0x00000000; //Logical Address Register
		vector<unsigned char>* readdata;
		
		//read Logical Address register
		unsigned int readlength=4; //4bytes-1word
		try{
			cout << "Reading Logical Address register...";
			readdata=rmapsocket->read(addressOfLogicaladdressregister,readlength);
			cout << "Done" << endl;
		}catch(RMAPException e){
			//if an exception occurs
			cout << "Exception in Read Access" << endl;
			//dump the exception
			e.dump();
		}
		//dump the result
		cout << "Logical Address Register : ";
		SpaceWireUtilities::dumpPacket(readdata,4);
		
		
		//read Receive Packet Counter registers
		for(unsigned int i=0;i<13;i++){
			try{
				unsigned int addressOfReceivedPacketCounter=0x060+i*4;
				cout << "Reading Receive Packet Counter register (port " << dec << i+1 << ")...";
				readdata=rmapsocket->read(addressOfReceivedPacketCounter,readlength);
				cout << "Done" << endl;
			}catch(RMAPException e){
				//if an exception occurs
				cout << "Exception in Read Access" << endl;
				//dump the exception
				e.dump();
			}
			//dump the result
			cout << "Received Packet Counter Register (port " << i+1 << ") : ";
			SpaceWireUtilities::dumpPacket(readdata,4);
		}
		
		//finalization
		rmapengine->closeRMAPSocket(rmapsocket);
		rmapengine->stop();
		spacewireif->close();
		delete rmapengine;
		delete spacewireif;				
	}
};

int main(int argc,char* argv[]){
	Main main;
	main.run();
	return 0;
}

/** History
 * 2008-05-xx file created (Takayuki Yuasa)
 * 2008-09-04 use SpaceWireIFImplementations::selectInstanceFromCommandLineMenu() (Takayuki Yuasa)
 */
