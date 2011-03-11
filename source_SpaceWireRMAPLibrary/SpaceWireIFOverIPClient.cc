#include "SpaceWireIFOverIPClient.hh"
#include "Condition.hh"

#include <stdlib.h>
using namespace std;

//#define DEBUG

SpaceWireIFOverIPClient::SpaceWireIFOverIPClient(string newiphostname,unsigned int newipportnumber) : SpaceWireIF(), iphostname(newiphostname), ipportnumber(newipportnumber){
	//cout << "SpaceWireIFOverIPClient::SpaceWireIFOverIPClient()" << endl;
}

SpaceWireIFOverIPClient::~SpaceWireIFOverIPClient(){}

bool SpaceWireIFOverIPClient::initialize() throw(SpaceWireException){
	initializationmutex.lock();
	initialized=true;
	timecodeaction=NULL;
	initializationmutex.unlock();
	return true;
}

bool SpaceWireIFOverIPClient::open() throw(SpaceWireException){
	try{
		datasocket=new IPClientSocket(iphostname,ipportnumber);
		datasocket->open();
		datasocket->setTimeout(5000);
	}catch(IPSocketException e){
		if(e.getStatus()=="IPSocket::receive() TimeOut"){
			throw SpaceWireException(SpaceWireException::TransferErrorTimeout);
		}else{
			cout << "SpaceWireIFOverIPClient::open() exception" << endl;
			exit(-1);
		}
	}

	ssdtp=new SSDTPModule(datasocket);
	ssdtp->setTimeCodeAction(this);
	//cout << "$$ " << ssdtp->receivedsize << endl;
	return true;
}

bool SpaceWireIFOverIPClient::open(int portnumber) throw(SpaceWireException){
	throw SpaceWireException(SpaceWireException::NotImplemented);
}

void SpaceWireIFOverIPClient::close(){
	datasocket->close();
	delete datasocket;
	delete ssdtp;
}

void SpaceWireIFOverIPClient::startLinkInitializationSequenceAsAMasterNode() throw(SpaceWireException){}
void SpaceWireIFOverIPClient::startLinkInitializationSequenceAsASlaveNode() throw(SpaceWireException){}
void SpaceWireIFOverIPClient::stopLink() throw(SpaceWireException){}

void SpaceWireIFOverIPClient::resetStateMachine() throw(SpaceWireException){}
void SpaceWireIFOverIPClient::clearSendFIFO() throw(SpaceWireException){}
void SpaceWireIFOverIPClient::clearReceiveFIFO() throw(SpaceWireException){}

bool SpaceWireIFOverIPClient::reset() throw(SpaceWireException){
	close();
	open();
	return true;
}

void SpaceWireIFOverIPClient::finalize() throw(SpaceWireException){
	SpaceWireIF::finalize();
}

void SpaceWireIFOverIPClient::send(vector<unsigned char>* packet) throw(SpaceWireException){
#ifdef DEBUG
	cout << "SpaceWireIFOverIPClient::send() invoked" << endl;
	if(this->isDebugMode()){
		SpaceWireUtilities::dumpPacket(packet);
	}
#endif
	ssdtp->send(packet);
#ifdef DEBUG
	cout << "SpaceWireIFOverIPClient::send() completed" << endl;
#endif
}

vector<unsigned char> SpaceWireIFOverIPClient::receive() throw(SpaceWireException){
#ifdef DEBUG
	cout << "SpaceWireIFOverIPClient::receive() invoked" << endl;
#endif
#ifdef DEBUG
	cout << "SpaceWireIFOverIPClient::receive() waiting" << endl;
#endif
	try {
		vector<unsigned char> packet=ssdtp->receive();
#ifdef DEBUG
		if(this->isDebugMode()){
			SpaceWireUtilities::dumpPacket(&packet);
		}
#endif
		return packet;
	}catch(SSDTPException e){
		throw SpaceWireException(SpaceWireException::Disconnected);
	}catch(IPSocketException e){
		throw SpaceWireException(SpaceWireException::Disconnected);
	}
}

void SpaceWireIFOverIPClient::receive(vector<unsigned char>* packet) throw(SpaceWireException){
#ifdef DEBUG
	cout << "SpaceWireIFOverIPClient::receive() invoked" << endl;
#endif
#ifdef DEBUG
	cout << "SpaceWireIFOverIPClient::receive() waiting" << endl;
#endif
	try {
		ssdtp->receive(packet);
#ifdef DEBUG
		if(this->isDebugMode()){
			SpaceWireUtilities::dumpPacket(packet);
		}
#endif
	}catch(SSDTPException e){
		throw SpaceWireException(SpaceWireException::Disconnected);
	}catch(IPSocketException e){
		throw SpaceWireException(SpaceWireException::Disconnected);
	}
}

void SpaceWireIFOverIPClient::abort() throw(SpaceWireException){

}

SpaceWireAsynchronousStatus SpaceWireIFOverIPClient::asynchronousReceive() throw(SpaceWireException){ return *(new SpaceWireAsynchronousStatus); }
SpaceWireAsynchronousStatus SpaceWireIFOverIPClient::asynchronousSend(vector<unsigned char>* packet) throw(SpaceWireException){ return *(new SpaceWireAsynchronousStatus); }
void SpaceWireIFOverIPClient::waitCompletionOfAsynchronousOperation(SpaceWireAsynchronousStatus status) throw(SpaceWireException){}

void SpaceWireIFOverIPClient::sendTimeCode(unsigned char flag_and_timecode) throw(SpaceWireException){
	ssdtp->sendTimeCode(flag_and_timecode);
}

unsigned char SpaceWireIFOverIPClient::getTimeCode() throw(SpaceWireException){
	return ssdtp->getTimeCode();
}

void SpaceWireIFOverIPClient::doAction(unsigned char timecode){
	if(timecodeaction!=NULL){
		timecodeaction->doAction(timecode);
	}else{
		cout << "SpaceWireIFOverIPClient::doAction(): Got TimeCode : " << hex << setw(2) << setfill('0') << (unsigned int)timecode << dec << endl;
	}
}

void SpaceWireIFOverIPClient::setTxDivCount(unsigned char txdivcount){
	ssdtp->setTxDivCount(txdivcount);
}

/** History
 * 2008-08-26 file created (Takayuki Yuasa)
 * 2008-12-17 added TimeCode related methods (Takayuki Yuasa)
 * 2009-07-15 debug mode added (Takayuki Yuasa)
 */
