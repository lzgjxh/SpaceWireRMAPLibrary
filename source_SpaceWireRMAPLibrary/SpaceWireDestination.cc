#include "SpaceWireDestination.hh"
using namespace std;

SpaceWireDestination::SpaceWireDestination(){
	setDestinationLogicalAddress(SpaceWireDestination::DefaultLogicalAddress);
	clearDestinationPathAddress();
}

SpaceWireDestination::SpaceWireDestination(unsigned char address){
	setDestinationLogicalAddress(address);
	clearDestinationPathAddress();
}

SpaceWireDestination::SpaceWireDestination(vector<unsigned char> address){
	setDestinationLogicalAddress(SpaceWireDestination::DefaultLogicalAddress);
	setDestinationPathAddress(address);
}

SpaceWireDestination::SpaceWireDestination(unsigned char logicaladdress,vector<unsigned char> pathaddress){
	setDestinationLogicalAddress(logicaladdress);
	setDestinationPathAddress(pathaddress);
}

void SpaceWireDestination::setDestinationLogicalAddress(unsigned char address){
	destinationLogicalAddress=address;
}

unsigned char SpaceWireDestination::getDestinationLogicalAddress(){
	return destinationLogicalAddress;
}

void SpaceWireDestination::setDestinationPathAddress(vector<unsigned char> address){
	destinationPathAddress=address;
}

void SpaceWireDestination::clearDestinationPathAddress(){
	destinationPathAddress.clear();
}

vector<unsigned char> SpaceWireDestination::getDestinationPathAddress(){
	return destinationPathAddress;
}
