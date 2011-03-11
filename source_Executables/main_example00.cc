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
		
		//create an instance of RMAPDestination
		//set properties for Shimafuji SpaceWire DIO Board
		RMAPDestination rmapdestination;
		rmapdestination.setDefaultDestination();
		rmapdestination.setDestinationLogicalAddress(0xFE);
		rmapdestination.setSourceLogicalAddress(0xFE);
		rmapdestination.setDestinationKey(0x02);
		rmapdestination.setDraftFCRC();
		vector<unsigned char> path;
		path.clear();
		rmapdestination.setDestinationPathAddress(path);
		rmapdestination.setSourcePathAddress(path);
		rmapdestination.setWordWidth(1);
		
		//open RMAPSocket to "rmapdestination"
		RMAPSocket* rmapsocket=rmapengine->openRMAPSocketTo(rmapdestination);
		
		//prepare data vector for write access
		unsigned int writelength=16;
		vector<unsigned char> writedata;
		for(unsigned int i=0;i<writelength;i++){
			writedata.push_back((unsigned char)i);
		}
		
		//execute RMAP Write to Shimafuji DIO Board's SDRAM
		unsigned int writeaddress=0x00000000; //SDRAM Address
		try{
			cout << "Writing...";
			rmapsocket->write(writeaddress,&writedata);
			cout << "Done" << endl;
		}catch(RMAPException e){
			//if an exception occurs
			cout << "Exception in Write Access" << endl;
			//dump the exception
			e.dump();
		}
		
		//execute RMAP Read to Shimafuji DIO Board's SDRAM
		unsigned int readaddress=0x00000000;
		unsigned int readlength=16;
		vector<unsigned char>* readdata;
		try{
			cout << "Reading...";
			readdata=rmapsocket->read(readaddress,readlength);
			cout << "Done" << endl;
		}catch(RMAPException e){
			//if an exception occurs
			cout << "Exception in Read Access" << endl;
			//dump the exception
			e.dump();
		}
		
		//dump the result
		cout << "Written Data : " << endl;
		SpaceWireUtilities::dumpPacket(&writedata);
		cout << "Read Data : " << endl;
		SpaceWireUtilities::dumpPacket(readdata);
		
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
