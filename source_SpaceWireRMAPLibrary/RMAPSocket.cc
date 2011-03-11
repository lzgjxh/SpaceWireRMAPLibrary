#include "RMAPSocket.hh"

#include <stdlib.h>
using namespace std;

//#define DEBUG

RMAPSocket::RMAPSocket(){
	initialize();
}

RMAPSocket::RMAPSocket(unsigned int newsocketid){
	initialize();
	setSocketID(newsocketid);
}

RMAPSocket::RMAPSocket(unsigned int newsocketid,RMAPEngine* newrmapengine){
	initialize();
	setSocketID(newsocketid);
	setParentRMAPEngine(newrmapengine);
#ifdef DEBUG
	cout << "RMAPSocket constructed" << endl;cout.flush();
#endif
}

RMAPSocket::RMAPSocket(unsigned int newsocketid,RMAPEngine* newrmapengine,RMAPDestination newrmapdestination){
	initialize();
	setSocketID(newsocketid);
	setParentRMAPEngine(newrmapengine);
	setRMAPDestination(newrmapdestination);
#ifdef DEBUG
	cout << "RMAPSocket constructed" << endl;cout.flush();
#endif
}

void RMAPSocket::initialize(){
	commandpacket=new RMAPPacket();
	replypacket=new RMAPPacket();
	socketid=RMAPSocket::DefaultSocketID;
	timeoutenabled=RMAPSocket::DefaultTimeoutMode;
	timeoutduration=RMAPSocket::DefaultTimeoutDurationInMilliSecond;
	automaticretryenabled=RMAPSocket::DefaultAutomaticRetryMode;
	crccheck=RMAPPacket::RMAPDefaultCRCCheckMode;
	applyCRCCheckMode();
	resetCounter();
	requestcounter=0;
}

vector<unsigned char>* RMAPSocket::read(unsigned int address,unsigned int length) throw(RMAPException){
	readmutex.lock();
	requestcounter++;
#ifdef DEBUG
	cout << "RMAPSocket read() start" << endl;cout.flush();
#endif
	//create "read command packet"
	commandpacket->setCommandPacket();
	commandpacket->setReadMode();
	commandpacket->setAddress(address);
	commandpacket->setLength(length);
#ifdef DEBUG
	cout << "RMAPSocket read() command packet set" << endl;
	cout.flush();
	commandpacket->dumpPacket();
#endif
	//Since createPacket() will be called in RMAPEngine after setting new Transaction ID,
	//the line below should be skipped to avoid double invokation.
	//commandpacket->createPacket();

	//tell request to RMAPEngine by passing command packet
#ifdef DEBUG
	cout << "RMAPSocket read() parentrmapengine->requestCommand()" << endl;cout.flush();
#endif
	sendrequest_read:
	parentrmapengine->requestCommand(this,commandpacket);
#ifdef DEBUG
	cout << "RMAPSocket read() parentrmapengine->requestCommand() done" << endl;cout.flush();
#endif

	//receive reply packet
#ifdef DEBUG
	cout << "RMAPSocket read() parentrmapengine->receiveReply()" << endl;cout.flush();
#endif
	RMAPPacket* tempreplypacket;
	try{
		tempreplypacket=parentrmapengine->receiveReply(this);
	}catch(RMAPException e){
#ifdef DEBUG
		cout << "### Exception in RMAPSocket Read()" << endl;
#endif
		if(e.getStatus()==RMAPException::Timedout && this->isAutomaticRetryEnabled()){
			goto sendrequest_read;
		}else{
			readmutex.unlock();
			throw e;
		}
	}
		//receiveReply can throw RMAPException (Timeout case)
	delete replypacket;
	replypacket=tempreplypacket; //change reference
	applyCRCCheckMode();

#ifdef DEBUG
	cout << "RMAPSocket read() parentrmapengine->receiveReply() done" << endl;cout.flush();
#endif
	//check reply status
	if(replypacket->getReplyStatus()==RMAPPacket::CommandExcecutedSuccessfully){
		//completed successfully
		readmutex.unlock();
		return replypacket->getData();
	}else{
		RMAPException e(RMAPException::RMAPUnsuccessfull);
		readmutex.unlock();
		throw e;
	}
}

void RMAPSocket::write(unsigned int address, vector<unsigned char>* data) throw(RMAPException){
	writemutex.lock();
	requestcounter++;
#ifdef DEBUG
	cout << "RMAPSocket::write() invoked" << endl;cout.flush();
#endif

	//create "write command packet"
	commandpacket->setCommandPacket();
	commandpacket->setWriteMode();
	commandpacket->setAddress(address);
	commandpacket->setData(data);
	//Since createPacket() will be called in RMAPEngine after setting new Transaction ID,
	//the line below should be skipped to avoid double invokation.
	//commandpacket->createPacket();

#ifdef DEBUG
	cout << "RMAPSocket::write() parentrmapengine->requestCommand" << endl;cout.flush();
#endif

	//send command packet
	sendrequest_write:
	parentrmapengine->requestCommand(this,commandpacket);

	//if ack mode
	if(commandpacket->isAckMode()){
		RMAPPacket* tempreplypacket;
		try{
			tempreplypacket=parentrmapengine->receiveReply(this);
				//receiveReply can throw RMAPException (Timeout case)
		}catch(RMAPException e){
#ifdef DEBUG
			cout << "### Exception in RMAPSocket Write()" << endl;
#endif
			if(e.getStatus()==RMAPException::Timedout && this->isAutomaticRetryEnabled()){
				goto sendrequest_write;
			}else{
				writemutex.unlock();
				throw e;
			}
		}
		delete replypacket;
		replypacket=tempreplypacket; //change reference
		applyCRCCheckMode();
#ifdef DEBUG
	cout << "RMAPSocket::write() Reply received" << endl;cout.flush();
#endif
		//check reply status
		if(replypacket->getReplyStatus()==0x00){
			//completed successfully
			writemutex.unlock();
			return;
		}else{
			RMAPException e(RMAPException::RMAPUnsuccessfull);
			writemutex.unlock();
			throw e;
		}
	}
}

void RMAPSocket::readModifyWrite(unsigned int address,vector<unsigned char>* data, unsigned int length) throw(RMAPException){
	requestcounter++;
	cout << "RMAPSocket::readModifyWrite : currently not implemented." << endl;
	exit(-1);
}

unsigned int RMAPSocket::getSocketID(){
	return socketid;
}

void RMAPSocket::setSocketID(unsigned int newsocketid){
	socketid=newsocketid;
}

RMAPEngine* RMAPSocket::getParentRMAPEngine(){
	return parentrmapengine;
}

void RMAPSocket::setParentRMAPEngine(RMAPEngine* newrmapengine){
	parentrmapengine=newrmapengine;
}

RMAPPacket RMAPSocket::getTheLastReplyPacket(){
	return *replypacket;
}

RMAPPacket RMAPSocket::getTheLastCommandPacket(){
	return *commandpacket;
}

void RMAPSocket::enableTimeout(unsigned int newtimeoutduration){
	timeoutenabled=RMAPSocket::TimeoutModeEnabled;
	timeoutduration=newtimeoutduration;
}

void RMAPSocket::enableTimeout(){
	timeoutenabled=RMAPSocket::TimeoutModeEnabled;
}

void RMAPSocket::disableTimeout(){
	timeoutenabled=RMAPSocket::TimeoutModeDisabled;
}

bool RMAPSocket::isTimeoutEnabled(){
	if(timeoutenabled==RMAPSocket::TimeoutModeEnabled){
		return true;
	}else{
		return false;
	}
}

unsigned int RMAPSocket::getTimeoutDuration() throw (RMAPException){
	if(isTimeoutEnabled()==true){
		return timeoutduration;
	}else{
		throw(RMAPException(RMAPException::TimeoutNotEnabled));
	}
}

void RMAPSocket::enableAutomaticRetry(){
	automaticretryenabled=RMAPSocket::AutomaticRetryEnabled;
}

void RMAPSocket::disableAutomaticRetry(){
	automaticretryenabled=RMAPSocket::AutomaticRetryDisabled;
}

bool RMAPSocket::isAutomaticRetryEnabled(){
	if(automaticretryenabled==RMAPSocket::AutomaticRetryEnabled){
		return true;
	}else{
		return false;
	}
}

void RMAPSocket::enableCRCCheck(){
	crccheck=true;
	applyCRCCheckMode();
}

void RMAPSocket::disableCRCCheck(){
	crccheck=false;
	applyCRCCheckMode();
}
