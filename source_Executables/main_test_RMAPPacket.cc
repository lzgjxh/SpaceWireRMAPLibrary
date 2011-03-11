/** Sample Program For RMAPPacket class.
 */

#include "RMAPPacket.hh"
#include "SpaceWireUtilities.hh"

#include <iostream>
#include <iomanip>
#include <vector>
using namespace std;

class Main {
public:
	Main(){
	}
	
	void run(){
		cout << ""
		 << "***************************************" << endl
		 << "* Example 1                            " << endl
		 << "* Create 'RMAP Write Command' packet   " << endl
		 << "* which is the same as the one shown in" << endl
		 << "* the RMAP standard (Page 44 of Draft F)" << endl
		 << "***************************************" << endl;
		RMAPDestination destination;
		destination.setDestinationKey(0x99);
		destination.setDestinationLogicalAddress(0x54);
		
		vector<unsigned char> destinationpathaddress;
		destinationpathaddress.clear(); // => means No Destination Path Address
		destination.setDestinationPathAddress(destinationpathaddress);
	
		vector<unsigned char> sourcepathaddress;
		sourcepathaddress.clear(); // => means No Source Path Address
		destination.setSourcePathAddress(sourcepathaddress);
		
		destination.setSourceLogicalAddress(0x76);
		destination.setDraftFCRC();
		
		vector<unsigned char> writedata;
		for(int i=0;i<16;i++){
			writedata.push_back(i); // => Prepare "Data" part
		}
		
		RMAPPacket packet;
		packet.setRMAPDestination(destination);
		packet.setWriteMode();
		packet.setNoVerifyMode();
		packet.setAckMode();
		packet.setNoIncrementMode();
		packet.setTransactionID(0x0006);
		packet.setExtendedAddress(0x01);
		packet.setAddress(0x00000000);
		packet.setData(&writedata);
		packet.createPacket();
		
		packet.dumpPacket();
		  // => dump the created "RMAP Write Command" packet
		
		cout << endl << endl;
		cout << ""
		 << "***************************************" << endl 
		 << "* Example 2                            " << endl
		 << "* Interpretation of a faked 'RMAP Read" << endl
		 << "* Command' packet.                     " << endl
		 << "***************************************" << endl;
		//fake Packet
		vector<unsigned char> writecommandpacketdata;
		//unsigned char fake1[]={0x54,0x01,0x4c,0x57,0x76,0x00,0x05,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x10,0xb9};
		unsigned char fake1[]={0x32,0x01,0x4d,0x02,0x00,0x00,0x00,0x04,0xfe,0x00,0x02,0x00,0xf0,0x00,0x00,0x00,0x00,0x00,0x10,0x98};
		for(int i=0;i<20;i++){
			writecommandpacketdata.push_back(fake1[i]);
		}
		RMAPPacket writecommandpacket;
		writecommandpacket.setPacket(&writecommandpacketdata);
		  // => set faked vector data to RMAPPacket instance
		  // => then interpret the packet which is set just above
		writecommandpacket.dumpPacket();
		  // => dump the result of packet interpretation
	
		cout << endl << endl;
		cout << ""
		 << "***************************************" << endl 
		 << "* Example 3                            " << endl
		 << "* Interpretation of a faked 'RMAP Write" << endl
		 << "* Reply' packet.                       " << endl
		 << "***************************************" << endl;
		//fake Packet
		vector<unsigned char> replypacketdata;
		unsigned char fake2[]={0x31,0x01,0x3d,0x00,0x44,0x00,0x00,0xdc};
		for(int i=0;i<8;i++){
			replypacketdata.push_back(fake2[i]);
		}
		RMAPPacket replypacket;
		replypacket.setPacket(&replypacketdata);
		  // => set faked vector data to RMAPPacket instance
		  // => then interpret the packet which is set just above
		replypacket.dumpPacket();
		  // => dump the result of packet interpretation
		
		cout << endl << endl;
		cout << ""
		 << "***************************************" << endl 
		 << "* Example 4                            " << endl
		 << "* Path Address                         " << endl
		 << "***************************************" << endl;
		vector<unsigned char> pathaddress;
		RMAPDestination destinationwithpathaddress;
		destinationpathaddress.clear();
		destinationpathaddress.push_back(0x01);
		sourcepathaddress.clear();
		sourcepathaddress.push_back(0x04);
		destinationwithpathaddress.setDestinationPathAddress(destinationpathaddress);
		destinationwithpathaddress.setSourcePathAddress(sourcepathaddress);
		cout << "Destoination Path Address" << endl;
		pathaddress = destinationwithpathaddress.getDestinationPathAddress();
		SpaceWireUtilities::dumpPacket(&pathaddress);
		cout << "Source Path Address" << endl;
		pathaddress = destinationwithpathaddress.getSourcePathAddress();
		SpaceWireUtilities::dumpPacket(&pathaddress);
		
		cout << endl;
		RMAPPacket packetwithpathaddresses;
		packetwithpathaddresses.setRMAPDestination(destinationwithpathaddress);
		packetwithpathaddresses.setCommandPacket();
		packetwithpathaddresses.setReadMode();
		packetwithpathaddresses.createPacket();
		cout << "Created Packet" << endl;
		SpaceWireUtilities::dumpPacket(packetwithpathaddresses.getPacket());
		
		cout << endl;
		cout << "Try to interpret the created packet" << endl;
		RMAPPacket trialpacket;
		trialpacket.setPacket(packetwithpathaddresses.getPacket());
		trialpacket.dump();
	}
};

int main(int argc,char** argv){
	Main main;
	main.run();
	return 0;
}

/** History
 * 2008-05-01 file created (Takayuki Yuasa)
 * 2008-10-04 path address test added (Takayuki Yuasa)
 * 
 */
