#include "SpaceWireAsynchronousStatus.hh"
using namespace std;

int SpaceWireAsynchronousStatus::CurrentAsynchronousProcessID;
map<int,bool> SpaceWireAsynchronousStatus::processIDMap;

SpaceWireAsynchronousStatus::SpaceWireAsynchronousStatus(){
	asynchronousOperationID=SpaceWireAsynchronousStatus::DefaultAsynchronousProcessID;
}

SpaceWireAsynchronousStatus::~SpaceWireAsynchronousStatus(){}

int SpaceWireAsynchronousStatus::getID(){
	return asynchronousOperationID;
}

void SpaceWireAsynchronousStatus::setLinkStatus(SpaceWireLinkStatus newlinkstatus){
	linkstatus=newlinkstatus;
}

SpaceWireLinkStatus SpaceWireAsynchronousStatus::getLinkStatus(){
	return linkstatus;
}


void SpaceWireAsynchronousStatus::waitCompletion() throw(SpaceWireException){
	SpaceWireException e(SpaceWireException::NotImplemented);
	throw e;
}

int SpaceWireAsynchronousStatus::newAsynchronousProcessID(){
	/**
	 * 
	 */
	if(CurrentAsynchronousProcessID==MaximumAsynchronousProcessID){
		CurrentAsynchronousProcessID=MinimumAsynchronousProcessID;
	}else{
		CurrentAsynchronousProcessID++;
	}

	return CurrentAsynchronousProcessID;
	/** 
	 * 
	 */
}

void SpaceWireAsynchronousStatus::clearAsynchronousProcessID(int id){
	/**
	 * 
	 */
	
}
