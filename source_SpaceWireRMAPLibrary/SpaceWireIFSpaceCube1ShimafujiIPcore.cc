#include "SpaceWireIFSpaceCube1ShimafujiIPcore.hh"

using namespace std;

SpaceWireIFSpaceCube1ShimafujiIPcore::SpaceWireIFSpaceCube1ShimafujiIPcore() : SpaceWireIF(){
	portnumber=SHIMAFUJI_IPCORE_DEFAULT_PORTNUMBER;
}

SpaceWireIFSpaceCube1ShimafujiIPcore::SpaceWireIFSpaceCube1ShimafujiIPcore(int newportnumber) : SpaceWireIF(){
	portnumber=newportnumber;
}


SpaceWireIFSpaceCube1ShimafujiIPcore::~SpaceWireIFSpaceCube1ShimafujiIPcore(){
}

bool SpaceWireIFSpaceCube1ShimafujiIPcore::initialize() throw(SpaceWireException){
	initializationmutex.lock();
	cout << "SpaceWireIFSpaceCube1ShimafujiIPcore : initialize" << endl;

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

bool SpaceWireIFSpaceCube1ShimafujiIPcore::open() throw(SpaceWireException){
	open(portnumber);
}

bool SpaceWireIFSpaceCube1ShimafujiIPcore::open(int newportnumber) throw(SpaceWireException){
	portnumber=newportnumber;
	int result=spw_open(newportnumber);
	devicedescriptor=result;
	cout << "SpaceWireIFSpaceCube1ShimafujiIPcore : open (" << result << ")" << endl;

	result=spw_connect(devicedescriptor);
	cout << "SpaceWireIFSpaceCube1ShimafujiIPcore : connect (" << result << ")" << endl;
	if(result<0){
		throw SpaceWireException(SpaceWireException::OpenFailed);
	}else{
		opened=true;
		return true;
	}
	setLinkStatus(linkstatus);
}

void SpaceWireIFSpaceCube1ShimafujiIPcore::close(){
	int result=spw_close(devicedescriptor);
	cout << "SpaceWireIFSpaceCube1ShimafujiIPcore : close (" << result << ")" << endl;
	opened=false;
}

void SpaceWireIFSpaceCube1ShimafujiIPcore::startLinkInitializationSequenceAsAMasterNode() throw(SpaceWireException){}

void SpaceWireIFSpaceCube1ShimafujiIPcore::startLinkInitializationSequenceAsASlaveNode() throw(SpaceWireException){}

void SpaceWireIFSpaceCube1ShimafujiIPcore::stopLink() throw(SpaceWireException){
	close();
}

SpaceWireLinkStatus SpaceWireIFSpaceCube1ShimafujiIPcore::getLinkStatus() throw(SpaceWireException){
	return linkstatus;
}

void SpaceWireIFSpaceCube1ShimafujiIPcore::setLinkStatus(SpaceWireLinkStatus newlinkstatus) throw(SpaceWireException){
	if(!opened){
		cout << "SpaceWireIFSpaceCube1ShimafujiIPcore::setLinkStatus exception : I/F is not opened yet" << endl;
		throw SpaceWireException(SpaceWireException::NotOpened);
	}
	int result;
	linkstatus=newlinkstatus;

	//set timeout durations
	result=spw_set_read_timeout(devicedescriptor,(unsigned int)linkstatus.getTimeout());
	result=spw_set_write_timeout(devicedescriptor,(unsigned int)linkstatus.getTimeout());
	if(result<0){
		cout << "SpaceWireIFSpaceCube1ShimafujiIPcore::setLinkStatus result:" << result << endl;
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

void SpaceWireIFSpaceCube1ShimafujiIPcore::resetStateMachine() throw(SpaceWireException){}
void SpaceWireIFSpaceCube1ShimafujiIPcore::clearSendFIFO() throw(SpaceWireException){}
void SpaceWireIFSpaceCube1ShimafujiIPcore::clearReceiveFIFO() throw(SpaceWireException){}

bool SpaceWireIFSpaceCube1ShimafujiIPcore::reset() throw(SpaceWireException){
	return false;
}

void SpaceWireIFSpaceCube1ShimafujiIPcore::finalize() throw(SpaceWireException){
	Sfree(sendbuffer);
	delete receivebuffer;
}

void SpaceWireIFSpaceCube1ShimafujiIPcore::send(vector<unsigned char>* packet) throw(SpaceWireException){
	//cout << "SpaceWireIFSpaceCube1ShimafujiIPcore::send() invoked" << endl;
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
	//cout << "SpaceWireIFSpaceCube1ShimafujiIPcore::send() completed" << endl;
}

vector<unsigned char> SpaceWireIFSpaceCube1ShimafujiIPcore::receive() throw(SpaceWireException){
	//cout << "SpaceWireIFSpaceCube1ShimafujiIPcore::receive() invoked" << endl;
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
	//cout << "SpaceWireIFSpaceCube1ShimafujiIPcore::receive() completed" << endl;
	return *receivebuffer;
}

void SpaceWireIFSpaceCube1ShimafujiIPcore::receive(vector<unsigned char>* packet) throw(SpaceWireException){
	//cout << "SpaceWireIFSpaceCube1ShimafujiIPcore::receive() invoked" << endl;
	unsigned int result=0;
	unsigned int size=0;
	packet->resize(BufferSize);
	result=spw_read(devicedescriptor,(UW*)&packet->at(0), &size);

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

void SpaceWireIFSpaceCube1ShimafujiIPcore::receive(unsigned char* array,unsigned int* receivedsize){
	unsigned int result=0;
	result=spw_read(devicedescriptor,(UW*)array, (UW*)receivedsize);

	if(result!=0 || *receivedsize<0 || *receivedsize>BufferSize){
		SpaceWireException e(SpaceWireException::ReceiveFailed);
		if(isDebugMode()){ //debug mode
			cout << "receive() exception: " << endl;
			cout << "result:" << dec << result << " size:" << dec << receivedsize << endl;
			e.dump();
		}
		throw e;
	}
}

void SpaceWireIFSpaceCube1ShimafujiIPcore::abort() throw(SpaceWireException){

}

SpaceWireAsynchronousStatus SpaceWireIFSpaceCube1ShimafujiIPcore::asynchronousReceive() throw(SpaceWireException){
	SpaceWireAsynchronousStatus as;
	return as;
}
SpaceWireAsynchronousStatus SpaceWireIFSpaceCube1ShimafujiIPcore::asynchronousSend(vector<unsigned char>* packet) throw(SpaceWireException){
	SpaceWireAsynchronousStatus as;
	return as;
}

void SpaceWireIFSpaceCube1ShimafujiIPcore::waitCompletionOfAsynchronousOperation(SpaceWireAsynchronousStatus status) throw(SpaceWireException){}

void SpaceWireIFSpaceCube1ShimafujiIPcore::sendTimeCode(unsigned char flag_and_timecode) throw(SpaceWireException){
	int result=spw_send_timecode(devicedescriptor,flag_and_timecode);
	if(result<0){
		cout << "TimeCode send error result:" << result << endl;
		throw SpaceWireException(SpaceWireException::SendTimeCodeError);
	}
}

unsigned char SpaceWireIFSpaceCube1ShimafujiIPcore::getTimeCode() throw(SpaceWireException){
	unsigned char flag_and_timecode=0;
	int result=spw_get_timecode(devicedescriptor,&flag_and_timecode);
	if(result<0){
		throw SpaceWireException(SpaceWireException::GetTimeCodeError);
	}
	return flag_and_timecode;
}


/** History
 * 2008-05-xx file created (Takayuki Yuasa)
 * 2008-12-17 added TimeCode related methods (Takayuki Yuasa)
 */
