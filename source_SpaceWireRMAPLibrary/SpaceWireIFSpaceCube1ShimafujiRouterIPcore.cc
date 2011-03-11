#include "SpaceWireIFSpaceCube1ShimafujiRouterIPcore.hh"

using namespace std;

SpaceWireIFSpaceCube1ShimafujiRouterIPcore::SpaceWireIFSpaceCube1ShimafujiRouterIPcore() : SpaceWireIF(){
	sendbuffer=(UW*)Smalloc(BufferSize*sizeof(UW));
	receivebuffer=new vector<unsigned char>(BufferSize);

	portnumber=SHIMAFUJI_ROUTERIPCORE_DEFAULT_PORTNUMBER;
}

SpaceWireIFSpaceCube1ShimafujiRouterIPcore::SpaceWireIFSpaceCube1ShimafujiRouterIPcore(unsigned int newport) : SpaceWireIF(){
	sendbuffer=(UW*)Smalloc(BufferSize*sizeof(UW));
	receivebuffer=new vector<unsigned char>(BufferSize);

	portnumber=newport;
}

SpaceWireIFSpaceCube1ShimafujiRouterIPcore::~SpaceWireIFSpaceCube1ShimafujiRouterIPcore(){
}

bool SpaceWireIFSpaceCube1ShimafujiRouterIPcore::initialize() throw(SpaceWireException){
	initializationmutex.lock();
	cout << "SpaceWireIFSpaceCube1ShimafujiRouterIPcore : initialize" << endl;
	if(initialized==false){
		initialized=true;
		opened=false;
		debugmode=false;
	}
	initializationmutex.unlock();

	return true;
}

bool SpaceWireIFSpaceCube1ShimafujiRouterIPcore::open() throw(SpaceWireException){
	return open(portnumber);
}

bool SpaceWireIFSpaceCube1ShimafujiRouterIPcore::open(int portnum) throw(SpaceWireException){
	portnumber=portnum;
	cout << "SpaceWireIFSpaceCube1ShimafujiRouterIPcore : open()" << endl;
	if(SHIMAFUJI_ROUTERIPCORE_PORTNUMBER_MINIMUM<=portnumber
			&& portnumber<=SHIMAFUJI_ROUTERIPCORE_PORTNUMBER_MAXIMUM){
		int result=spwr_open(portnumber);
		if(result>0){
			cout << "SpaceWireIFSpaceCube1ShimafujiRouterIPcore : open Port " << portnumber << " open (" << result << ")" << endl;
			opened=true;
			devicedescriptor=result;

			SpaceWireLinkStatus ls;
			ls.setTimeout(1000); //1s timeout
			ls.setLinkSpeed(100);
			setLinkStatus(ls);

			//set link speed of port 1,2,3 to 100MHz (initially 10MHz)
			cout << "SpaceWireIFSpaceCube1ShimafujiRouterIPcore::open() Port 1,2,3 to 100MHz" << endl;
			result=spwr_set_txspeed(devicedescriptor,1,TXSPD_100MBPS);
			result=spwr_set_txspeed(devicedescriptor,2,TXSPD_100MBPS);
			result=spwr_set_txspeed(devicedescriptor,3,TXSPD_100MBPS);

			if(result<0){
				cout << "SpaceWireIFSpaceCube1ShimafujiRouterIPcore::open() could not change Port 1,2,3 link speed to 100MHz" << endl;
			}
			return true;
		}else{
			throw(SpaceWireException(SpaceWireException::OpenFailed));
		}
	}else{
		throw(SpaceWireException(SpaceWireException::OpenFailed));
	}
}

void SpaceWireIFSpaceCube1ShimafujiRouterIPcore::close(){
	int result=spwr_close(devicedescriptor);
	cout << "SpaceWireIFSpaceCube1ShimafujiRouterIPcore : close (" << result << ")" << endl;
	opened=false;
}

void SpaceWireIFSpaceCube1ShimafujiRouterIPcore::startLinkInitializationSequenceAsAMasterNode() throw(SpaceWireException){
	throw SpaceWireException(SpaceWireException::NotImplemented);
}

void SpaceWireIFSpaceCube1ShimafujiRouterIPcore::startLinkInitializationSequenceAsASlaveNode() throw(SpaceWireException){
	startLinkInitializationSequenceAsAMasterNode();
}

void SpaceWireIFSpaceCube1ShimafujiRouterIPcore::stopLink() throw(SpaceWireException){
	close();
}

SpaceWireLinkStatus SpaceWireIFSpaceCube1ShimafujiRouterIPcore::getLinkStatus() throw(SpaceWireException){
	//comment
	stringstream ss;
	int result;
	unsigned int status;
	result=spwr_get_linkstatus(devicedescriptor,portnumber,&status);
	ss << "Control-Status Register:" << setfill('0') << setw(8) << hex << status;
	linkstatus.setComment(ss.str());

	return linkstatus;
}

void SpaceWireIFSpaceCube1ShimafujiRouterIPcore::setLinkStatus(SpaceWireLinkStatus newlinkstatus) throw(SpaceWireException){
	if(!opened){
		cout << "SpaceWireIFSpaceCube1ShimafujiIPcore::setLinkStatus exception : I/F is not opened yet" << endl;
		throw SpaceWireException(SpaceWireException::NotOpened);
	}
	int result;
	linkstatus=newlinkstatus;

	//set timeout durations
	result=spwr_set_read_timeout(devicedescriptor,(unsigned int)linkstatus.getTimeout());
	result=spwr_set_write_timeout(devicedescriptor,(unsigned int)linkstatus.getTimeout());
	if(result<0){
		cout << "SpaceWireIFSpaceCube1ShimafujiRouterIPcore::setLinkStatus result:" << result << endl;
		throw SpaceWireException(SpaceWireException::DeviceDriverFailed);
	}

	//set link speed
	int speed=linkstatus.getLinkSpeed();
	spwr_txclk_div txclk_div;
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
		result=spwr_set_txspeed(devicedescriptor,portnumber,txclk_div);
	}
}

void SpaceWireIFSpaceCube1ShimafujiRouterIPcore::resetStateMachine() throw(SpaceWireException){}
void SpaceWireIFSpaceCube1ShimafujiRouterIPcore::clearSendFIFO() throw(SpaceWireException){}
void SpaceWireIFSpaceCube1ShimafujiRouterIPcore::clearReceiveFIFO() throw(SpaceWireException){}

bool SpaceWireIFSpaceCube1ShimafujiRouterIPcore::reset() throw(SpaceWireException){
	int result=spwr_reset_link(devicedescriptor,portnumber);
	if(result<0){
		throw SpaceWireException(SpaceWireException::DeviceDriverFailed);
	}else{
		return true;
	}
}

void SpaceWireIFSpaceCube1ShimafujiRouterIPcore::finalize() throw(SpaceWireException){
	Sfree(sendbuffer);
	delete receivebuffer;
}

void SpaceWireIFSpaceCube1ShimafujiRouterIPcore::send(vector<unsigned char>* packet) throw(SpaceWireException){
	//cout << "SpaceWireIFSpaceCube1ShimafujiRouterIPcore::send() invoked" << endl;
	unsigned char* p = (unsigned char*)&sendbuffer[0];
	unsigned int size = packet->size();
	for (unsigned int i = 0; i < size; i++) {
		*p++ = packet->at(i);
	}
	if(isDebugMode()){ //debug mode
		cout << "send() on port " << portnumber <<" : " << endl;
		SpaceWireUtilities::dumpPacket(packet);
	}
	int result;
	result=spwr_write(devicedescriptor,sendbuffer, size);
	if(result<0){
		throw SpaceWireException(SpaceWireException::SendFailed);
	}
	//cout << "SpaceWireIFSpaceCube1ShimafujiRouterIPcore::send() completed" << endl;
}

vector<unsigned char> SpaceWireIFSpaceCube1ShimafujiRouterIPcore::receive() throw(SpaceWireException){
	//cout << "SpaceWireIFSpaceCube1ShimafujiRouterIPcore::receive() invoked" << endl;
	int result;
	unsigned int size=0;
	result=spwr_read(devicedescriptor,(UW*)&receivebuffer->at(0), &size);
	if(result<0){
		throw SpaceWireException(SpaceWireException::ReceiveFailed);
	}else{
		receivebuffer->resize(size);
	}
	if(isDebugMode()){ //debug mode
		cout << "receive() on port " << portnumber <<" : " << endl;
		SpaceWireUtilities::dumpPacket(receivebuffer);
	}
	//cout << "SpaceWireIFSpaceCube1ShimafujiRouterIPcore::receive() completed" << endl;
	return *receivebuffer;
}

void SpaceWireIFSpaceCube1ShimafujiRouterIPcore::receive(vector<unsigned char>* packet) throw(SpaceWireException){
	//cout << "SpaceWireIFSpaceCube1ShimafujiRouterIPcore::receive() invoked" << endl;
	int result=0;
	unsigned int size=0;
	packet->resize(BufferSize);
	result=spwr_read(devicedescriptor,(UW*)&packet->at(0), &size);
	if(size!=0){
		packet->resize(size);
	}
	if(result<0){
		throw SpaceWireException(SpaceWireException::ReceiveFailed);
	}
	if(isDebugMode()){ //debug mode
		cout << "receive() on port " << portnumber <<" : " << endl;
		SpaceWireUtilities::dumpPacket(packet);
	}
	//cout << "SpaceWireIFSpaceCube1ShimafujiRouterIPcore::receive() completed" << endl;
}

void SpaceWireIFSpaceCube1ShimafujiRouterIPcore::abort() throw(SpaceWireException){

}

SpaceWireAsynchronousStatus SpaceWireIFSpaceCube1ShimafujiRouterIPcore::asynchronousReceive() throw(SpaceWireException){
	SpaceWireAsynchronousStatus as;
	return as;
}
SpaceWireAsynchronousStatus SpaceWireIFSpaceCube1ShimafujiRouterIPcore::asynchronousSend(vector<unsigned char>* packet) throw(SpaceWireException){
	SpaceWireAsynchronousStatus as;
	return as;
}

void SpaceWireIFSpaceCube1ShimafujiRouterIPcore::waitCompletionOfAsynchronousOperation(SpaceWireAsynchronousStatus status) throw(SpaceWireException){}

void SpaceWireIFSpaceCube1ShimafujiRouterIPcore::sendTimeCode(unsigned char flag_and_timecode) throw(SpaceWireException){
	int result=spwr_send_timecode(devicedescriptor,flag_and_timecode);
	if(result<0){
		throw SpaceWireException(SpaceWireException::SendTimeCodeError);
	}
}

unsigned char SpaceWireIFSpaceCube1ShimafujiRouterIPcore::getTimeCode() throw(SpaceWireException){
	unsigned char flag_and_timecode=0;
	int result=spwr_get_timecode(devicedescriptor,&flag_and_timecode);
	if(result<0){
		throw SpaceWireException(SpaceWireException::GetTimeCodeError);
	}
	return flag_and_timecode;
}



/** History
 * 2008-09-16 file created (Takayuki Yuasa)
 * 2008-12-17 added TimeCode related methods (Takayuki Yuasa)
 */
