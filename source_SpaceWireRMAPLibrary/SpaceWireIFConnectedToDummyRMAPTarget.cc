#include "SpaceWireIFConnectedToDummyRMAPTarget.hh"
using namespace std;

//#define DEBUG
#define DUMP

SpaceWireIFConnectedToDummyRMAPTarget::SpaceWireIFConnectedToDummyRMAPTarget() : SpaceWireIF(){}
SpaceWireIFConnectedToDummyRMAPTarget::~SpaceWireIFConnectedToDummyRMAPTarget(){}

bool SpaceWireIFConnectedToDummyRMAPTarget::initialize() throw(SpaceWireException){
	initializationmutex.lock();
	initialized=true;
	timecode=0;
	initializationmutex.unlock();
	return true;
}

bool SpaceWireIFConnectedToDummyRMAPTarget::open() throw(SpaceWireException){
	return true;
}

bool SpaceWireIFConnectedToDummyRMAPTarget::open(int portnumber) throw(SpaceWireException){
	return true;
}

void SpaceWireIFConnectedToDummyRMAPTarget::close(){}
void SpaceWireIFConnectedToDummyRMAPTarget::startLinkInitializationSequenceAsAMasterNode() throw(SpaceWireException){}
void SpaceWireIFConnectedToDummyRMAPTarget::startLinkInitializationSequenceAsASlaveNode() throw(SpaceWireException){}
void SpaceWireIFConnectedToDummyRMAPTarget::stopLink() throw(SpaceWireException){}

void SpaceWireIFConnectedToDummyRMAPTarget::resetStateMachine() throw(SpaceWireException){}
void SpaceWireIFConnectedToDummyRMAPTarget::clearSendFIFO() throw(SpaceWireException){}
void SpaceWireIFConnectedToDummyRMAPTarget::clearReceiveFIFO() throw(SpaceWireException){}

bool SpaceWireIFConnectedToDummyRMAPTarget::reset() throw(SpaceWireException){
	SpaceWireIF::reset();
	return true;
}

void SpaceWireIFConnectedToDummyRMAPTarget::finalize() throw(SpaceWireException){
	SpaceWireIF::finalize();
}

void SpaceWireIFConnectedToDummyRMAPTarget::send(vector<unsigned char>* packet) throw(SpaceWireException){
#ifdef DEBUG
	cout << "SpaceWireIFConnectedToDummyRMAPTarget::send() invoked" << endl;
#endif
	RMAPPacket commandpacket;
	commandpacket.setPacket(packet);

#ifdef DUMP
	cout << "SpaceWireIFConnectedToDummyRMAPTarget send() : sent packet dump" << endl;
	commandpacket.dumpPacket();
	cout << "SpaceWireIFConnectedToDummyRMAPTarget send() : RMAPDestination of sent packet" << endl;
	commandpacket.getRMAPDestination()->dump();
#endif

	RMAPPacket* replypacket=new RMAPPacket();
	replypacket->setRMAPDestination(*(commandpacket.getRMAPDestination()));

	replypacket->setReplyStatus(0x00); //successfull
	replypacket->setReplyPacket();
	replypacket->setWriteOrRead(commandpacket.getWriteOrRead());
	replypacket->setTransactionID(commandpacket.getTransactionID());
	replypacket->setVerifyMode(commandpacket.getVerifyMode());
	replypacket->setVerifyMode(commandpacket.getVerifyMode());
	replypacket->setAckMode(commandpacket.getAckMode());
	replypacket->setIncrementMode(commandpacket.getIncrementMode ());
	replypacket->setExtendedAddress(commandpacket.getExtendedAddress());
	replypacket->setAddress(commandpacket.getAddress());
	replypacket->setLength(commandpacket.getLength());
	if(commandpacket.isReadMode()){
		vector<unsigned char> data;
		for(unsigned int i=0;i<commandpacket.getLength();i++){
			data.push_back((unsigned char)(i%0x100));
		}
		replypacket->setData(&data);
	}
	replypacket->createPacket();
	packetqueuemutex.lock();
	packetqueue.push(replypacket);
#ifdef DUMP
	cout << "SpaceWireIFConnectedToDummyRMAPTarget send() : dump Reply Packet" << endl;
	SpaceWireUtilities::dumpPacket(replypacket->getPacket());
#endif
	condition.signal();
#ifdef DEBUG
	cout << "SpaceWireIFConnectedToDummyRMAPTarget send() : signaling to waiting threads" << endl;
#endif
	packetqueuemutex.unlock();
#ifdef DEBUG
	cout << "SpaceWireIFConnectedToDummyRMAPTarget::send() completed" << endl;
#endif
}

vector<unsigned char> SpaceWireIFConnectedToDummyRMAPTarget::receive() throw(SpaceWireException){
#ifdef DEBUG
	cout << "SpaceWireIFConnectedToDummyRMAPTarget::receive() invoked" << endl;
#endif
	while(true){
		if(packetqueue.size()!=0){
			packetqueuemutex.lock();
			RMAPPacket* replypacket=packetqueue.front();
			packetqueue.pop();
			packetqueuemutex.unlock();
			vector<unsigned char> replyvector;

			replyvector=*(replypacket->getPacket());
#ifdef DUMP
			cout << "SpaceWireIFConnectedToDummyRMAPTarget::receive() dump reply packet" << endl;
			replypacket->dumpPacket();
#endif
			delete replypacket;
			cout << "SpaceWireIFConnectedToDummyRMAPTarget::receive() completed" << endl;
			return replyvector;
		}
#ifdef DEBUG
		cout << "SpaceWireIFConnectedToDummyRMAPTarget::receive() waiting" << endl;
#endif
		condition.wait(10);
	}
}

void SpaceWireIFConnectedToDummyRMAPTarget::receive(vector<unsigned char>* packet) throw(SpaceWireException){
	*packet=receive();
}

void SpaceWireIFConnectedToDummyRMAPTarget::abort() throw(SpaceWireException){

}

SpaceWireAsynchronousStatus SpaceWireIFConnectedToDummyRMAPTarget::asynchronousReceive() throw(SpaceWireException){ return *(new SpaceWireAsynchronousStatus); }
SpaceWireAsynchronousStatus SpaceWireIFConnectedToDummyRMAPTarget::asynchronousSend(vector<unsigned char>* packet) throw(SpaceWireException){ return *(new SpaceWireAsynchronousStatus); }
void SpaceWireIFConnectedToDummyRMAPTarget::waitCompletionOfAsynchronousOperation(SpaceWireAsynchronousStatus status) throw(SpaceWireException){}

void SpaceWireIFConnectedToDummyRMAPTarget::sendTimeCode(unsigned char flag_and_timecode) throw(SpaceWireException){
	timecode=flag_and_timecode;
}

unsigned char SpaceWireIFConnectedToDummyRMAPTarget::getTimeCode() throw(SpaceWireException){
	return timecode;
}

/** History
 * 2008-05-xx file created (Takayuki Yuasa)
 * 2008-12-17 added TimeCode related methods (Takayuki Yuasa)
 */
