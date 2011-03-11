#include "SpaceWireRTPacket.hh"
using namespace std;

void SpaceWireRTPacket::createPacket(){
	header.clear();
	SpaceWireRTDestination* spacewirertdestination=spacewirertchannel->getSpaceWireRTDestination();
	vector<unsigned char> pathaddress=spacewirertdestination.getDestinationPathAddress()
	header.push_back()
}

vector<unsigned char>* SpaceWireRTPacket::getPacket(){
	
}

void SpaceWireRTPacket::setPacket(vector<unsigned char>* newpacket) throw(SpaceWireRTException){
	
}

void SpaceWireRTPacket::setPacketBuffer(vector<unsigned char>* newpacketbuffer) throw(SpaceWireRTException){
	delete packet;
	packet=newpacketbuffer;
}

void SpaceWireRTPacket::interpretPacket() throw(SpaceWireRTException){
	try{
		//index
		int i=0;
		
		/** rtindex is an index of the Destination Logical Address
		 * item in a packet array.
		 */
		int rtindex=0;
		
	}catch(...){
#ifdef DEBUG
		cout << "SpaceWireRTPacket::interpretPacket() This is not a SpaceWire-RT packet." << endl;
#endif
		throw SpaceWireRTException(SpaceWireRTException::NotASpaceWireRTPacket);
	}
}

/** History
 * 2008-08-26 file created (Takayuki Yuasa)
 */