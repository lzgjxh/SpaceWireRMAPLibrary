/** Sample program for the testing of SpaceWireIF class.
 * The implementation which is to be tested is specified
 * in the instantiation sentence (new SpaceWireIFxxxx()).
 */

#include "RMAPPacket.hh"
#include "SpaceWireIF.hh"
#include "SpaceWireIFSpaceCube1ShimafujiIPcore.hh"
#include "SpaceWireUtilities.hh"

#include <iostream>
using namespace std;

int main(int argc,char* argv[]){
	SpaceWireIF* spacewireif=new SpaceWireIFSpaceCube1ShimafujiIPcore();
	spacewireif->initialize();
	spacewireif->open();
	
	cout << ""
	 << "***************************************" << endl
	 << "* Example 1                            " << endl
	 << "* RMAP Write Command                   " << endl
	 << "***************************************" << endl;
	RMAPPacket* writecommandpacket=new RMAPPacket();
	
	RMAPDestination dioboard;
	dioboard.setDestinationKey(0x02);
		
	writecommandpacket->setRMAPDestination(dioboard);
	writecommandpacket->setCommandPacket();
	writecommandpacket->setWriteMode();
	writecommandpacket->setAddress(0x00000000);
	vector<unsigned char> writedata;
	for(int i=0;i<16;i++){
		writedata.push_back((unsigned char)i);
	}
	writecommandpacket->setData(&writedata);
	writecommandpacket->createPacket();
	
	cout << "Write Command Packet is..." << endl;
	writecommandpacket->dumpPacket();

	cout << "Sending the Write Command packet...";
	spacewireif->send(writecommandpacket->getPacket());
	cout << "done" << endl;

	cout << "Waiting a reply packet...";
	vector<unsigned char> writereplypacketvector=spacewireif->receive();
	cout << "done" << endl;
	
	RMAPPacket* writereplypacket=new RMAPPacket();
	writereplypacket->setPacket(&writereplypacketvector);

	cout << "Reply packet is..." << endl;
	writereplypacket->dumpPacket();

	
	cout << ""
	 << "***************************************" << endl
	 << "* Example 2                            " << endl
	 << "* RMAP Read Command                    " << endl
	 << "***************************************" << endl;
	RMAPPacket* readcommandpacket=new RMAPPacket();
	
	readcommandpacket->setRMAPDestination(dioboard);
	
	readcommandpacket->setCommandPacket();
	readcommandpacket->setReadMode();
	readcommandpacket->setAddress(0x00000000);
	readcommandpacket->setLength(16);
	readcommandpacket->createPacket();
	
	cout << "Read Command packet is..." << endl;
	readcommandpacket->dumpPacket();
	
	cout << "Sending the Read Command packet...";
	spacewireif->send(readcommandpacket->getPacket());
	cout << "done" << endl;
	
	cout << "Waiting a reply packet...";
	vector<unsigned char> readreplypacketvector=spacewireif->receive();
	cout << "done" << endl;
	
	RMAPPacket* readreplypacket=new RMAPPacket();
	readreplypacket->setPacket(&readreplypacketvector);
	
	cout << "Reply packet is..." << endl;
	readreplypacket->dumpPacket();
	
	delete spacewireif;
	delete writecommandpacket;
	delete writereplypacket;
	delete readcommandpacket;
	delete readreplypacket;
	return 0;
}