#include "SpaceWireIF.hh"

using namespace std;

bool SpaceWireIF::initialized=false;

SpaceWireIF::SpaceWireIF(){
	portnumber=1;
	initialized=false;
}

SpaceWireIF::~SpaceWireIF(){
	
}

bool SpaceWireIF::initialize() throw(SpaceWireException){
	initializationmutex.lock();
	initialized=true;
	debugmode=false;
	initializationmutex.unlock();
	return true;
}

void SpaceWireIF::finalize() throw(SpaceWireException){
}

bool SpaceWireIF::reset() throw(SpaceWireException){
	return true;
}

int SpaceWireIF::getPortNumber(){
	return portnumber;
}

SpaceWireLinkStatus SpaceWireIF::getLinkStatus() throw(SpaceWireException){
	return linkstatus;
}

void SpaceWireIF::setLinkStatus(SpaceWireLinkStatus newstatus) throw(SpaceWireException){
	linkstatus=newstatus;
}

void SpaceWireIF::setTimeCodeAction(SpaceWireTimeCodeAction* action){
	timecodeaction=action;
}
