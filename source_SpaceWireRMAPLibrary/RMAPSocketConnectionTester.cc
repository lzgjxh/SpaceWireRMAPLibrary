/*
 * RMAPSocketConnectionTester.cc
 *
 *  Created on: 2010/06/08
 *      Author: yuasa
 */

#include "RMAPSocketConnectionTester.hh"
using namespace std;

RMAPSocketConnectionTester::RMAPSocketConnectionTester(RMAPSocket* rmapsocket,unsigned int address,unsigned int length){
	this->rmapsocket=rmapsocket;
	this->address=address;
	this->length=length;
	duration=1000;
	started=false;
}

RMAPSocketConnectionTester::~RMAPSocketConnectionTester(){
	if(started==true && succeeded==false){
		rmapsocket->getParentRMAPEngine()->cancelRequestedCommand(rmapsocket->getSocketID());
	}
}

void RMAPSocketConnectionTester::setTimeoutDuration(unsigned int duration){
	this->duration=duration;
}

void RMAPSocketConnectionTester::run(){
	started=true;
	succeeded=false;
	rmapsocket->enableTimeout(duration);
	rmapsocket->enableAutomaticRetry();
	while(!succeeded){
		try{
			vector<unsigned char>* data=rmapsocket->read(address,length);
			if(data->size()==length){succeeded=true;}
		}catch(RMAPException e){e.dump();}
	}
	started=false;
}

void RMAPSocketConnectionTester::stop(){
	rmapsocket->getParentRMAPEngine()->cancelRequestedCommand(rmapsocket->getSocketID());
	started=false;
}

bool RMAPSocketConnectionTester::isSucceeded(){
	return succeeded;
}
