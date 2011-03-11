#include "SpaceWireIFZestSC1ShimafujiIPCoreSinglePort.hh"

using namespace std;

SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::SpaceWireIFZestSC1ShimafujiIPCoreSinglePort() : SpaceWireIF(){
	portnumber=SHIMAFUJI_IPCORE_DEFAULT_PORTNUMBER;
}

SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::SpaceWireIFZestSC1ShimafujiIPCoreSinglePort(int newportnumber) : SpaceWireIF(){
	portnumber=newportnumber;
}


SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::~SpaceWireIFZestSC1ShimafujiIPCoreSinglePort(){
}

bool SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::initialize() throw(SpaceWireException){
	initializationmutex.lock();
	cout << "SpaceWireIFZestSC1ShimafujiIPCoreSinglePort : initialize" << endl;

	sendbuffer=(UW*)Smalloc(BufferSize*sizeof(UW));
	for(int i=0;i<BufferSize;i++){
		sendbuffer[i]=0;
	}
	//receivebuffer=(UW*)Smalloc(BufferSize*sizeof(UW));
	receivebuffer=new vector<unsigned char>(BufferSize);

	linkstatus.setLinkSpeed(100);
	linkstatus.setTimeout(1000);

	initialized=true;
	opened=false;
	debugmode=false;

	initializationmutex.unlock();
	return true;
}

bool SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::open() throw(SpaceWireException){
	open(portnumber);
}

bool SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::open(int newportnumber) throw(SpaceWireException){
	portnumber=newportnumber;
	int result=spw_open(newportnumber);
	devicedescriptor=result;
	cout << "SpaceWireIFZestSC1ShimafujiIPCoreSinglePort : open (" << result << ")" << endl;

	result=spw_connect(devicedescriptor);
	cout << "SpaceWireIFZestSC1ShimafujiIPCoreSinglePort : connect (" << result << ")" << endl;
	if(result<0){
		throw SpaceWireException(SpaceWireException::OpenFailed);
	}else{
		opened=true;
		return true;
	}
	setLinkStatus(linkstatus);
}

void SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::close(){
	int result=spw_close(devicedescriptor);
	cout << "SpaceWireIFZestSC1ShimafujiIPCoreSinglePort : close (" << result << ")" << endl;
	opened=false;
}

void SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::startLinkInitializationSequenceAsAMasterNode() throw(SpaceWireException){}

void SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::startLinkInitializationSequenceAsASlaveNode() throw(SpaceWireException){}

void SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::stopLink() throw(SpaceWireException){
	close();
}

SpaceWireLinkStatus SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::getLinkStatus() throw(SpaceWireException){
	return linkstatus;
}

void SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::setLinkStatus(SpaceWireLinkStatus newlinkstatus) throw(SpaceWireException){
	if(!opened){
		cout << "SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::setLinkStatus exception : I/F is not opened yet" << endl;
		throw SpaceWireException(SpaceWireException::NotOpened);
	}
	int result;
	linkstatus=newlinkstatus;

	//set timeout durations
	result=spw_set_read_timeout(devicedescriptor,(unsigned int)linkstatus.getTimeout());
	result=spw_set_write_timeout(devicedescriptor,(unsigned int)linkstatus.getTimeout());
	if(result<0){
		cout << "SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::setLinkStatus result:" << result << endl;
		throw SpaceWireException(SpaceWireException::DeviceDriverFailed);
	}

	//set link speed
	int speed=linkstatus.getLinkSpeed();
	spw_txclk_div txclk_div;
	switch(speed){
	case 100: txclk_div=TXSPD_100MBPS;break;
	case 50: txclk_div=TXSPD_50MBPS;break;
	case 33: txclk_div=TXSPD_33MBPS;break;
	case 25: txclk_div=TXSPD_25MBPS;break;
	case 20: txclk_div=TXSPD_20MBPS;break;
	case 10: txclk_div=TXSPD_10MBPS;break;
	case 5: txclk_div=TXSPD_5MBPS;break;
	case 2: txclk_div=TXSPD_2MBPS;break;
	}

	if(txclk_div==-1){
		throw SpaceWireException(SpaceWireException::DeviceDriverFailed);
	}else{
		result=spw_set_txspeed(devicedescriptor,txclk_div);
	}
}

void SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::resetStateMachine() throw(SpaceWireException){}
void SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::clearSendFIFO() throw(SpaceWireException){}
void SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::clearReceiveFIFO() throw(SpaceWireException){}

bool SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::reset() throw(SpaceWireException){
	return false;
}

void SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::finalize() throw(SpaceWireException){
	Sfree(sendbuffer);
	delete receivebuffer;
}

void SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::send(vector<unsigned char>* packet) throw(SpaceWireException){
	//cout << "SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::send() invoked" << endl;
	unsigned char* p = (unsigned char*)&sendbuffer[0];
	unsigned int size = packet->size();
	for (unsigned int i = 0; i < size; i++) {
		*p++ = packet->at(i);
	}
	if(isDebugMode()){ //debug mode
	  cout << "send() : " << endl;
	  SpaceWireUtilities::dumpPacket(packet);
	}
	spw_write(devicedescriptor,sendbuffer, size);
	//cout << "SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::send() completed" << endl;
}

vector<unsigned char> SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::receive() throw(SpaceWireException){
	//cout << "SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::receive() invoked" << endl;
	unsigned int result=0;
	unsigned int size=0;
	receivebuffer->resize(BufferSize);
	result=spw_read(devicedescriptor,(UW*)&receivebuffer->at(0), &size);

	if(result==0 && size>0 && size<=BufferSize){
		receivebuffer->resize(size);
	}else{
		SpaceWireException e(SpaceWireException::ReceiveFailed);
		if(isDebugMode()){ //debug mode
			cout << "receive() exception: " << endl;
			cout << "result:" << dec << result << " size:" << dec << size << endl;
			e.dump();
		}
		throw e;
	}

	if(isDebugMode()){ //debug mode
		cout << "receive() : " << endl;
		cout << "result:" << dec << result << " size:" << dec << size << endl;
		SpaceWireUtilities::dumpPacket(receivebuffer);
	}
	//cout << "SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::receive() completed" << endl;
	return *receivebuffer;
}

void SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::receive(vector<unsigned char>* packet) throw(SpaceWireException){
	//cout << "SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::receive() invoked" << endl;
	unsigned int result=0;
	unsigned int size=0;
	packet->resize(BufferSize);

	//check if at least one side of a double buffer is ready (full,EOP, or EEP)
	unsigned char value;
	ZestSC1ReadRegister(handler,AddressOfReceiveBufferReadyRegister,value);

	if()


	if(result==0 && size>0 && size<=BufferSize){
		packet->resize(size);
	}else{
		SpaceWireException e(SpaceWireException::ReceiveFailed);
		if(isDebugMode()){ //debug mode
			cout << "receive() exception: " << endl;
			cout << "result:" << dec << result << " size:" << dec << size << endl;
			e.dump();
		}
		throw e;
	}

	if(isDebugMode()){ //debug mode
		cout << "receive() : " << endl;
		cout << "result:" << dec << result << " size:" << dec << size << endl;
		SpaceWireUtilities::dumpPacket(packet);
	}
}

void SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::abort() throw(SpaceWireException){

}

SpaceWireAsynchronousStatus SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::asynchronousReceive() throw(SpaceWireException){
	SpaceWireAsynchronousStatus as;
	return as;
}
SpaceWireAsynchronousStatus SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::asynchronousSend(vector<unsigned char>* packet) throw(SpaceWireException){
	SpaceWireAsynchronousStatus as;
	return as;
}

void SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::waitCompletionOfAsynchronousOperation(SpaceWireAsynchronousStatus status) throw(SpaceWireException){}

void SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::sendTimeCode(unsigned char flag_and_timecode) throw(SpaceWireException){
	int result=spw_send_timecode(devicedescriptor,flag_and_timecode);
	if(result<0){
		cout << "TimeCode send error result:" << result << endl;
		throw SpaceWireException(SpaceWireException::SendTimeCodeError);
	}
}

unsigned char SpaceWireIFZestSC1ShimafujiIPCoreSinglePort::getTimeCode() throw(SpaceWireException){
	unsigned char flag_and_timecode=0;
	int result=spw_get_timecode(devicedescriptor,&flag_and_timecode);
	if(result<0){
		throw SpaceWireException(SpaceWireException::GetTimeCodeError);
	}
	return flag_and_timecode;
}


/** History
 * 2009-07-17 file created (Takayuki Yuasa)
 */
