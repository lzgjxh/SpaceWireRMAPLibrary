#include "SpaceWireIFDomesticPipe.hh"
using namespace std;

SpaceWireIFDomesticPipe::SpaceWireIFDomesticPipe(SpaceWireIFDomesticPipeCore* newcore, unsigned int newpipeendtype) : SpaceWireIF(){
	domesticpipecore=newcore;
	pipeendtype=newpipeendtype;
}

SpaceWireIFDomesticPipe::~SpaceWireIFDomesticPipe(){}

bool SpaceWireIFDomesticPipe::initialize() throw(SpaceWireException){
	return true;
}

bool SpaceWireIFDomesticPipe::open() throw(SpaceWireException){
	return true;
}

bool SpaceWireIFDomesticPipe::open(int portnumber) throw(SpaceWireException){
	return true;
}

void SpaceWireIFDomesticPipe::close(){}
void SpaceWireIFDomesticPipe::startLinkInitializationSequenceAsAMasterNode() throw(SpaceWireException){}
void SpaceWireIFDomesticPipe::startLinkInitializationSequenceAsASlaveNode() throw(SpaceWireException){}
void SpaceWireIFDomesticPipe::stopLink() throw(SpaceWireException){}

void SpaceWireIFDomesticPipe::resetStateMachine() throw(SpaceWireException){}
void SpaceWireIFDomesticPipe::clearSendFIFO() throw(SpaceWireException){}
void SpaceWireIFDomesticPipe::clearReceiveFIFO() throw(SpaceWireException){}

bool SpaceWireIFDomesticPipe::reset() throw(SpaceWireException){
	return true;
}

void SpaceWireIFDomesticPipe::finalize() throw(SpaceWireException){
}

void SpaceWireIFDomesticPipe::send(vector<unsigned char>* packet) throw(SpaceWireException){
	domesticpipecore->pushPacket(pipeendtype,packet);
}

vector<unsigned char> SpaceWireIFDomesticPipe::receive() throw(SpaceWireException){
	return domesticpipecore->popPacket(pipeendtype);
}

void SpaceWireIFDomesticPipe::receive(vector<unsigned char>* packet) throw(SpaceWireException){
	*packet=receive();
}

void SpaceWireIFDomesticPipe::abort() throw(SpaceWireException){}

SpaceWireAsynchronousStatus SpaceWireIFDomesticPipe::asynchronousReceive() throw(SpaceWireException){ return *(new SpaceWireAsynchronousStatus); }
SpaceWireAsynchronousStatus SpaceWireIFDomesticPipe::asynchronousSend(vector<unsigned char>* packet) throw(SpaceWireException){ return *(new SpaceWireAsynchronousStatus); }
void SpaceWireIFDomesticPipe::waitCompletionOfAsynchronousOperation(SpaceWireAsynchronousStatus status) throw(SpaceWireException){}

void SpaceWireIFDomesticPipe::sendTimeCode(unsigned char flag_and_timecode) throw(SpaceWireException){
	domesticpipecore->setTimeCode(flag_and_timecode);
}

unsigned char SpaceWireIFDomesticPipe::getTimeCode() throw(SpaceWireException){
	return domesticpipecore->getTimeCode();
}

/** History
 * 2008-09-xx file created (Takayuki Yuasa)
 * 2008-12-17 added TimeCode related methods (Takayuki Yuasa)
 */
