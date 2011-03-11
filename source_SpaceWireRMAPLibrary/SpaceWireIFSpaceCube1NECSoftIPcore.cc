#ifdef USE_SPC1_NEC_IP_CORE

#include "SpaceWireIFSpaceCube1NECSoftIPcore.hh"

#include "Condition.hh"

using namespace std;

SpaceWireIFSpaceCube1NECSoftIPcore::SpaceWireIFSpaceCube1NECSoftIPcore() : SpaceWireIF(){
	portnumber=DEFAULT_PORTNUMBER;
}

SpaceWireIFSpaceCube1NECSoftIPcore::SpaceWireIFSpaceCube1NECSoftIPcore(unsigned int newportnumber){
	portnumber=newportnumber;
}

SpaceWireIFSpaceCube1NECSoftIPcore::~SpaceWireIFSpaceCube1NECSoftIPcore(){
	Sfree(sendbuffer);
	Sfree(receivebuffer);
}


bool SpaceWireIFSpaceCube1NECSoftIPcore::initialize() throw(SpaceWireException){
	initializationmutex.lock();
	if(initialized==false){
		cout << "SpaceWireIFSpaceCube1NECSoftIPcore : initialize" << endl;
		sendbuffer=Smalloc(SendBufferSize);
		receivebuffer=Smalloc(ReceiveBufferSize);
		initialized=true;
		unsetDebugMode();
	}
	initializationmutex.unlock();
	return true;
}

bool SpaceWireIFSpaceCube1NECSoftIPcore::open() throw(SpaceWireException){
	if(portnumber<PORTNUMBER_MINIMUM || PORTNUMBER_MAXIMUM<portnumber){
		cout << "SpaceWireIFSpaceCube1NECSoftIPcore : port number (" << portnumber << ") error" << endl;
		return false;
	}

	bool result=SpaceWire_Open(&spwhandler,portnumber);
	cout << "SpaceWireIFSpaceCube1NECSoftIPcore : port " << portnumber << " initialize (" << ((result)? "successful":"failed") << ")" << endl;
	if(result==false){
		throw(SpaceWireException(SpaceWireException::OpenFailed));
	}
	//register receive mode on "portnumber"
	cout << "SpaceWireIFSpaceCube1NECSoftIPcore : receive port registering...";cout.flush();
	result=SpaceWire_RegisterReceiveOnAllPorts(spwhandler);
	cout << "done" << endl;cout.flush();
	opened=result;
	return result;
}

bool SpaceWireIFSpaceCube1NECSoftIPcore::open(int newportnumber) throw(SpaceWireException){
	portnumber=newportnumber;
	open();
}

void SpaceWireIFSpaceCube1NECSoftIPcore::close(){
	SpaceWire_Close(spwhandler);
	cout << "SpaceWireIFSpaceCube1NECSoftIPcore : port " << portnumber << " close" << endl;
	opened=false;
}

void SpaceWireIFSpaceCube1NECSoftIPcore::startLinkInitializationSequenceAsAMasterNode() throw(SpaceWireException){}

void SpaceWireIFSpaceCube1NECSoftIPcore::startLinkInitializationSequenceAsASlaveNode() throw(SpaceWireException){}

void SpaceWireIFSpaceCube1NECSoftIPcore::stopLink() throw(SpaceWireException) throw(SpaceWireException){
	close();
}

SpaceWireLinkStatus SpaceWireIFSpaceCube1NECSoftIPcore::setLinkStatus() throw(SpaceWireException){
	return linkstatus;
}

void SpaceWireIFSpaceCube1NECSoftIPcore::setLinkStatus(SpaceWireLinkStatus newstatus) throw(SpaceWireException){
	SpaceWireIF::setLinkStatus(newstatus);
	//set timeout
	double timeoutInMilliSec=getLinkStatus().getTimeout();
	SpaceWire_SetTimeout(spwhandler,timeoutInMilliSec*1000);
}

void SpaceWireIFSpaceCube1NECSoftIPcore::resetStateMachine() throw(SpaceWireException){}
void SpaceWireIFSpaceCube1NECSoftIPcore::clearSendFIFO() throw(SpaceWireException){}
void SpaceWireIFSpaceCube1NECSoftIPcore::clearReceiveFIFO() throw(SpaceWireException){}

bool SpaceWireIFSpaceCube1NECSoftIPcore::reset() throw(SpaceWireException){
	return false;
}

void SpaceWireIFSpaceCube1NECSoftIPcore::finalize() throw(SpaceWireException){}

void SpaceWireIFSpaceCube1NECSoftIPcore::send(vector<unsigned char>* packet) throw(SpaceWireException){
	unsigned int size=packet->size();
	for(unsigned int i=0;i<size;i++){
		((unsigned char*)sendbuffer)[i]=packet->at(i);
	}
	SPACEWIRE_STATUS status;
	SPACEWIRE_ID* packetid=(SPACEWIRE_ID*)Smalloc(sizeof(SPACEWIRE_ID));
	if(isDebugMode()){ //debug mode
		cout << "send() : " << endl;
		SpaceWireUtilities::dumpPacket(packet);
	}
	status=SpaceWire_SendPacket(spwhandler,sendbuffer,packet->size(),false,packetid);
	if(status!=TRANSFER_STARTED){
		throw(SpaceWireException(SpaceWireException::SendFailed));
	}

	status=SpaceWire_WaitOnSendCompleting(spwhandler,*packetid,true);
	if(status!=TRANSFER_SUCCESS){
		throw(SpaceWireException(SpaceWireException::SendFailed));
	}
	if(isDebugMode()){ //debug mode
		cout << "send() packet id : " << dec << *packetid << endl;
	}
	SpaceWire_FreeSend(spwhandler,*packetid);
	Sfree(packetid);
}

vector<unsigned char> SpaceWireIFSpaceCube1NECSoftIPcore::sendreceive(vector<unsigned char>* packet) throw(SpaceWireException){
	unsigned int size=packet->size();
	for(unsigned int i=0;i<size;i++){
		((unsigned char*)sendbuffer)[i]=packet->at(i);
		cout << setfill('0') << setw(2) << hex << (unsigned int)packet->at(i) << endl;
	}
	SPACEWIRE_STATUS statussend;
	SPACEWIRE_ID* packetidsend=(SPACEWIRE_ID*)Smalloc(sizeof(SPACEWIRE_ID));

	SPACEWIRE_STATUS statusreceive;
	SPACEWIRE_PACKET_PROPERTIES* propertiesreceive=(SPACEWIRE_PACKET_PROPERTIES*)Smalloc(sizeof(SPACEWIRE_PACKET_PROPERTIES));
	SPACEWIRE_ID* packetidreceive=(SPACEWIRE_ID*)Smalloc(sizeof(SPACEWIRE_ID));

	statussend=SpaceWire_SendPacket(spwhandler,sendbuffer,packet->size(),false,packetidsend);
	if(statussend!=TRANSFER_STARTED){
		throw(SpaceWireException(SpaceWireException::SendFailed));
	}
	Condition c;
	cout << "condition wait" << endl;
	c.wait(1000);
	cout << "condition wait completed" << endl;
	statusreceive=SpaceWire_ReadPackets(spwhandler,receivebuffer,ReceiveBufferSize,1,false,propertiesreceive,packetidreceive);
	cout << "waiting" << endl;cout.flush();
	statusreceive=SpaceWire_WaitOnReadCompleting(spwhandler,*packetidreceive,false);

	if(statusreceive!=TRANSFER_SUCCESS){
		throw(SpaceWireException(SpaceWireException::ReceiveFailed));
	}
	vector<unsigned char> receivedpacket;
	unsigned int receivedsize=(unsigned int)propertiesreceive->len;
	for(unsigned int i=0;i<receivedsize;i++){
		receivedpacket.push_back(((unsigned char*)receivebuffer)[i]);
	}

	SpaceWire_FreeRead(spwhandler,*packetidreceive);
	Sfree(propertiesreceive);
	Sfree(packetidreceive);
	SpaceWire_FreeSend(spwhandler,*packetidsend);
	Sfree(packetidsend);
	return receivedpacket;
}

vector<unsigned char> SpaceWireIFSpaceCube1NECSoftIPcore::receive() throw(SpaceWireException){
	SPACEWIRE_STATUS status;
	SPACEWIRE_PACKET_PROPERTIES* properties=(SPACEWIRE_PACKET_PROPERTIES*)Smalloc(sizeof(SPACEWIRE_PACKET_PROPERTIES));
	SPACEWIRE_ID* packetid=(SPACEWIRE_ID*)Smalloc(sizeof(SPACEWIRE_ID));

	status=SpaceWire_ReadPackets(spwhandler,receivebuffer,ReceiveBufferSize,1,false,properties,packetid);
	status=SpaceWire_WaitOnReadCompleting(spwhandler,*packetid,true);

	if(status!=TRANSFER_SUCCESS){
		throw(SpaceWireException(SpaceWireException::ReceiveFailed));
	}
	vector<unsigned char> packet;
	unsigned int size=(unsigned int)properties->len;
	for(unsigned int i=0;i<size;i++){
		packet.push_back(((unsigned char*)receivebuffer)[i]);
	}
	if(isDebugMode()){ //debug mode
		cout << "receive() : " << endl;
		SpaceWireUtilities::dumpPacket(&packet);
		cout << "receive() packet id : " << dec << *packetid << endl;
	}
	SpaceWire_FreeRead(spwhandler,*packetid);
	Sfree(properties);
	Sfree(packetid);
	return packet;
}

void SpaceWireIFSpaceCube1NECSoftIPcore::receive(vector<unsigned char>* packet) throw(SpaceWireException){
	*packet=receive();
}

void SpaceWireIFSpaceCube1NECSoftIPcore::abort() throw(SpaceWireException){

}

SpaceWireAsynchronousStatus SpaceWireIFSpaceCube1NECSoftIPcore::asynchronousReceive() throw(SpaceWireException){
	SpaceWireAsynchronousStatus as;
	return as;
}
SpaceWireAsynchronousStatus SpaceWireIFSpaceCube1NECSoftIPcore::asynchronousSend(vector<unsigned char>* packet)	throw(SpaceWireException){
	SpaceWireAsynchronousStatus as;
	return as;
}
void SpaceWireIFSpaceCube1NECSoftIPcore::waitCompletionOfAsynchronousOperation(SpaceWireAsynchronousStatus status) throw(SpaceWireException){}

void SpaceWireIFSpaceCube1NECSoftIPcore::sendTimeCode(unsigned char flag_and_timecode) throw(SpaceWireException){
	throw SpaceWireException(SpaceWireException::NotImplemented);
}

unsigned char SpaceWireIFSpaceCube1NECSoftIPcore::getTimeCode() throw(SpaceWireException){
	throw SpaceWireException(SpaceWireException::NotImplemented);
}


#endif

/** History
 * 2008-05-xx file created (Takayuki Yuasa)
 * 2008-12-17 added dummy TimeCode-related methods (Takayuki Yuasa)
 */
