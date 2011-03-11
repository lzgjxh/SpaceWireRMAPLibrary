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
				
		//create an instance of RMAPDestination
		//set properties for MHI SpaceWire Universal IO Board
		RMAPDestination rmapdestination;
		rmapdestination.setDefaultDestination();
		rmapdestination.setDestinationLogicalAddress(0x33);
		rmapdestination.setSourceLogicalAddress(0x31); // J106 Connector
		rmapdestination.setDestinationKey(0x00);
		rmapdestination.setDraftFCRC();
		vector<unsigned char> path;
		path.clear();
		rmapdestination.setDestinationPathAddress(path);
		rmapdestination.setSourcePathAddress(path);
		rmapdestination.setWordWidth(2);

		//prepare data vector for write access
		unsigned int writelength=16;
		vector<unsigned char> writedata;
		for(unsigned int i=0;i<writelength;i++){
			writedata.push_back((unsigned char)i);
		}
		
		//execute RMAP Write to Shimafuji DIO Board's SDRAM
		unsigned int writeaddress=0x00000000; //SDRAM Address
		
		RMAPPacket writecommandpacket;
		writecommandpacket.setRMAPDestination(rmapdestination);
		writecommandpacket.setCommandPacket();
		writecommandpacket.setWriteMode();
		writecommandpacket.setAddress(writeaddress);
		writecommandpacket.setData(&writedata);
		writecommandpacket.createPacket();
		
		try{
			//send RMAP Command Packet
			cout << "Sending Write Command Packet...";
			spacewireif->send(writecommandpacket.getPacket());
			cout << "Done" << endl;
			//receive RMAP Reply Packet
			cout << "Receiving Write Reply Packet...";
			vector<unsigned char> writereplypacketvector=spacewireif->receive();
			cout << "Done" << endl;
			RMAPPacket writereplypacket;
			writereplypacket.setPacket(&writereplypacketvector);
			if(writereplypacket.getReplyStatus()==RMAPPacket::CommandExcecutedSuccessfully){
				cout << "Successfully Wrote" << endl;
			}else{
				cout << "Failed" << endl;
				exit(-1);
			}
		}catch(SpaceWireException e){
			//if an exception occurs
			cout << "Exception in Write Access" << endl;
			//dump the exception
			e.dump();
		}
		
		//execute RMAP Read to Shimafuji DIO Board's SDRAM
		unsigned int readaddress=0x00000000;
		unsigned int readlength=16;
		
		RMAPPacket readcommandpacket;
		readcommandpacket.setRMAPDestination(rmapdestination);
		readcommandpacket.setCommandPacket();
		readcommandpacket.setReadMode();
		readcommandpacket.setAddress(readaddress);
		readcommandpacket.setLength(readlength);
		readcommandpacket.createPacket();
		
		vector<unsigned char> readdata;
		try{
			//send RMAP Command Packet
			cout << "Sending Read Command Packet...";
			spacewireif->send(readcommandpacket.getPacket());
			cout << "Done" << endl;
			//receive RMAP Reply Packet
			cout << "Receiving Read Reply Packet...";
			vector<unsigned char> readreplypacketvector=spacewireif->receive();
			cout << "Done" << endl;
			RMAPPacket readreplypacket;
			readreplypacket.setPacket(&readreplypacketvector);
			if(readreplypacket.getReplyStatus()==RMAPPacket::CommandExcecutedSuccessfully){
				cout << "Successfully Read" << endl;
				readdata=(*readreplypacket.getData()); //copy received data
			}else{
				cout << "Failed" << endl;
				exit(-1);
			}
		}catch(SpaceWireException e){
			//if an exception occurs
			cout << "Exception in Read Access" << endl;
			//dump the exception
			e.dump();
		}
		
		//dump the result
		cout << "Written Data : " << endl;
		SpaceWireUtilities::dumpPacket(&writedata);
		cout << "Read Data : " << endl;
		SpaceWireUtilities::dumpPacket(&readdata);
		
		//finalization
		spacewireif->close();
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
