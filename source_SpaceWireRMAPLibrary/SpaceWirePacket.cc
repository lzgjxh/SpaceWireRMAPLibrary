#include "SpaceWirePacket.hh"
using namespace std;

SpaceWirePacket::SpaceWirePacket(){
	packet=new vector<unsigned char>;
}

SpaceWirePacket::~SpaceWirePacket(){
	delete packet;
}

void SpaceWirePacket::setDestination(SpaceWireDestination newdestination){
	destination=newdestination;
}

SpaceWireDestination SpaceWirePacket::getDestination(){
	return destination;
}

void SpaceWirePacket::setPacket(vector<unsigned char>* newpacket){
	*packet=*newpacket; //copy byte-to-byte
}

void SpaceWirePacket::setPacketBuffer(vector<unsigned char>* newpacketbuffer){
	delete packet;
	packet=newpacketbuffer; //change reference
}

vector<unsigned char>* SpaceWirePacket::getPacket(){
	return packet;
}

vector<unsigned char>* SpaceWirePacket::getPacketBuffer(){
	return packet;
}

