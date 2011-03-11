#include "RMAPEngine.hh"
#include "SpaceWireUtilities.hh"

#include <stdlib.h>
using namespace std;

//#define DEBUG
//#define DUMP

RMAPEngine::RMAPEngine(){
	initialize();
	setSpaceWireIF(NULL);
}

RMAPEngine::RMAPEngine(SpaceWireIF* newspacewireif){
#ifdef DEBUG
	cout << "RMAPEngine constructed" << endl;cout.flush();
#endif
	initialize();
	setSpaceWireIF(newspacewireif);
}

RMAPEngine::~RMAPEngine(){

}

void RMAPEngine::initialize(){
#ifdef DEBUG
	cout << "RMAPEngine::initialize() invoked" << endl;cout.flush();
#endif
	suspended=true;
	packetreceivingoutsidetidscheme=true;
	initializeLists();
}

void RMAPEngine::start(){
#ifdef DEBUG
	cout << "RMAPEngine::start() invoked" << endl;cout.flush();
#endif
	suspended=false;
	replyprocessthread=
		new RMAPEngine_ReplyProcessThread(
				getSpaceWireIF(),this,&tidmapmutex,&replymapmutex,&replyloopcondition,
				&tidmap,&replymap,
				&destinationmap,&destinationmapmutex,
				&receivedpacketqueue,&receivedpacketqueuemutex,
				&coutmutex,&startstopmutex_send);
	replyprocessthread->start();
	Condition c;
	c.wait(2000);
	requestprocessthread=
		new RMAPEngine_RequestProcessThread(
				getSpaceWireIF(),this,&tidmapmutex,&requestmapmutex,&requestloopcondition,
				&tidmap,&requestmap,
				&destinationmap,&destinationmapmutex,
				&sentpacketqueue,&sentpacketqueuemutex,
				&coutmutex,&startstopmutex_receive);
	requestprocessthread->start();
}

void RMAPEngine::suspend(){
	suspended=true;
	requestprocessthread->suspend();
	replyprocessthread->suspend();

}

void RMAPEngine::resume(){
	requestprocessthread->resume();
	replyprocessthread->resume();
	suspended=false;
}

void RMAPEngine::stop(){
	class RMAPEngineThreadStopper : public Thread {
	private:
		Thread* thread;
	public:
		bool finished;
		RMAPEngineThreadStopper(Thread* thread_to_stop){
			thread = thread_to_stop;
			finished = false;
		}
		void run(){
			thread->join();
			finished = true;
		}
	};

	int i;
	Condition c;
	startstopmutex_send.lock();
	RMAPEngineThreadStopper stopper_request(requestprocessthread);
	stopper_request.start();
	requestprocessthread->stop();
	startstopmutex_send.unlock();
	cout << "Stopping RMAPEngine_RequestProcessThread ...";
	for (i = 0; i < 10; i++) {
	// while(stopper_request.finished==false);{
		if ( true == stopper_request.finished ) break;
		c.wait(100);
	}
	if ( 10 == i ) {
		stopper_request.cancel();
		// requestprocessthread->cancel();
		cout << " CANCELD" << endl;
	} else {
		cout << " OK" << endl;
		delete requestprocessthread;
	}

	startstopmutex_receive.lock();
	RMAPEngineThreadStopper stopper_reply(replyprocessthread);
	stopper_reply.start();
	replyprocessthread->stop();
	startstopmutex_receive.unlock();
	cout << "Stopping RMAPEngine_ReplyProcessThread ...";
	for (i = 0; i < 10; i++) {
	// while(stopper_reply.finished==false);{
		if ( true == stopper_reply.finished ) break;
		c.wait(100);
	}
	if ( 10 == i ) {
		stopper_reply.cancel();
		// replyprocessthread->cancel();
		cout << " CANCELED" << endl;
	} else {
		cout << " OK" << endl;
		delete replyprocessthread;
	}
}

RMAPSocket* RMAPEngine::openRMAPSocketTo(RMAPDestination newdestination) throw (RMAPException){
	openclosemutex.lock();
	unsigned int id=findAvailableSocketID();
	RMAPDestination* destination=new RMAPDestination();
	(*destination)=newdestination; //copy byte-by-byte
	RMAPSocket* socket=new RMAPSocket(id,this,*destination);
	socketmapmutex.lock();
	socketmap.insert(pair<unsigned int,RMAPSocket*>(id,socket));
	socketmapmutex.unlock();
	openclosemutex.unlock();
	destinationmapmutex.lock();
	destinationmap.insert(pair<unsigned int,RMAPDestination*>(id,destination));
	destinationmapmutex.unlock();
	return socket;
}

void RMAPEngine::closeRMAPSocket(RMAPSocket* socket){
	openclosemutex.lock();
	unsigned int id=socket->getSocketID();
	destinationmapmutex.lock();
	RMAPDestination* destination=(destinationmap.find(id))->second;
	delete destination;
	destinationmap.erase(id);
	destinationmapmutex.unlock();
	socketmapmutex.lock();
	socketmap.erase(id);
	socketmapmutex.unlock();
	delete socket;
	putBackSocketID(id);
	openclosemutex.unlock();
}


void RMAPEngine::requestCommand(RMAPSocket* socket, RMAPPacket* commandpacket) throw (RMAPException){
	if(suspended==true){
		throw(RMAPException(RMAPException::RMAPEngine_MultipleTransactionScheme_Suspended));
	}
#ifdef DEBUG
		coutmutex.lock();cout << "RMAPEngine::requestCommand() invoked" << endl;coutmutex.unlock();
#endif
	requestmapmutex.lock();
	requestmap.insert(pair<unsigned int,RMAPPacket*>(socket->getSocketID(),commandpacket));
	requestmapmutex.unlock();
#ifdef DEBUG
		coutmutex.lock();cout << "RMAPEngine::requestCommand() signaling" << endl;coutmutex.unlock();
#endif
	requestloopcondition.signal();
#ifdef DEBUG
		coutmutex.lock();cout << "RMAPEngine::requestCommand() completed" << endl;coutmutex.unlock();
#endif
}

void RMAPEngine::requestCommand(unsigned int socketid, RMAPPacket* commandpacket) throw (RMAPException){
	if(suspended==true){
		throw(RMAPException(RMAPException::RMAPEngine_MultipleTransactionScheme_Suspended));
	}
#ifdef DEBUG
		coutmutex.lock();cout << "RMAPEngine::requestCommand() invoked" << endl;coutmutex.unlock();
#endif
	requestmapmutex.lock();
	requestmap.insert(pair<unsigned int,RMAPPacket*>(socketid,commandpacket));
	requestmapmutex.unlock();
#ifdef DEBUG
		coutmutex.lock();cout << "RMAPEngine::requestCommand() signaling" << endl;coutmutex.unlock();
#endif
	requestloopcondition.signal();
#ifdef DEBUG
		coutmutex.lock();cout << "RMAPEngine::requestCommand() completed" << endl;coutmutex.unlock();
#endif
}

RMAPPacket* RMAPEngine::receiveReply(RMAPSocket* socket) throw (RMAPException){
#ifdef DEBUG
		coutmutex.lock();cout << "RMAPEngine::receiveReply()" << endl;coutmutex.unlock();
#endif
	unsigned int socketid=socket->getSocketID();
	unsigned int elapsedtime=0;
	unsigned int timeoutduration=0;
	if(socket->isTimeoutEnabled()){
		timeoutduration=socket->getTimeoutDuration();
	}

	while(true){
		if(suspended==true){
			throw(RMAPException(RMAPException::RMAPEngine_MultipleTransactionScheme_Suspended));
		}
		replymapmutex.lock();
		map<unsigned int,RMAPPacket*>::iterator replymap_find=replymap.find(socketid);
		if(replymap_find!=replymap.end()){
#ifdef DEBUG
		coutmutex.lock();cout << "RMAPEngine::receiveReply() found Reply Packet in replymap..." << endl;coutmutex.unlock();
#endif
			RMAPPacket* replypacket=replymap_find->second;
			replymap.erase(socketid);
			replymapmutex.unlock();
			return replypacket;
		}else{
			//could not find the reply packet in replymap

			//check if timeout occurs
			if(socket->isTimeoutEnabled()==true){
				if(elapsedtime<timeoutduration){
					elapsedtime=elapsedtime+RMAPEngine::ReceiveWaitMilliSecond;
				}else{
#ifdef DEBUG
		coutmutex.lock();cout << "RMAPEngine::receiveReply() Timeout Occurs (socketid=" << socketid << ")" << endl;coutmutex.unlock();
#endif
					//Timeout occurs
					replymapmutex.unlock();
					//cancel Request
					cancelRequestedCommand(socketid);
					//throw Time Out exception
#ifdef DEBUG
		coutmutex.lock();cout << "RMAPEngine::receiveReply() throwing RMAPException::TimedOut..." << endl;coutmutex.unlock();
#endif
					throw(RMAPException(RMAPException::Timedout));
				}
			}
			replymapmutex.unlock();
			//then wait meanwhile
			//socket->condition->wait(RMAPEngine::ReceiveWaitMilliSecond);
#ifdef DEBUG
		coutmutex.lock();cout << "RMAPEngine::receiveReply() wait a while" << endl;coutmutex.unlock();
#endif
			replyloopcondition.wait(RMAPEngine::ReceiveWaitMilliSecond);
		}
	}
}

RMAPPacket RMAPEngine::receiveReply(unsigned int socketid) throw (RMAPException){
	socketmapmutex.lock();
	RMAPSocket* socket=socketmap.find(socketid)->second;
	socketmapmutex.unlock();
	return *(receiveReply(socket));
}

void RMAPEngine::cancelRequestedCommand(unsigned int socketid) throw (RMAPException){
#ifdef DEBUG
		coutmutex.lock();cout << "RMAPEngine::cancelRequestedCommand()" << endl;coutmutex.unlock();
#endif
#ifdef DEBUG
		coutmutex.lock();cout << "RMAPEngine::cancelRequestedCommand() delete request" << endl;coutmutex.unlock();
#endif
	//Part 1 : delete Request from requestmap
	requestmapmutex.lock();
	map<unsigned int, RMAPPacket*>::iterator requestmap_find=requestmap.find(socketid);
	map<unsigned int, RMAPPacket*>::iterator requestmap_end=requestmap.end();
	if(requestmap_find!=requestmap_end){
		requestmap.erase(socketid);
	}
	requestmapmutex.unlock();

#ifdef DEBUG
		coutmutex.lock();cout << "RMAPEngine::cancelRequestedCommand() delete waiting tid" << endl;coutmutex.unlock();
#endif
	//Part 2 : delete Reply Wait from tidmap
	tidmapmutex.lock();
	vector<unsigned int> tid_to_be_deleted;
	bool found=false;
	//search tid used for this socketid
	map<unsigned int,unsigned int>::iterator tidmap_iterator=tidmap.begin();
	map<unsigned int,unsigned int>::iterator tidmap_end=tidmap.end();
	while(tidmap_iterator!=tidmap_end){
		if(tidmap_iterator->second==socketid){ //found
			found=true;
			tid_to_be_deleted.push_back(tidmap_iterator->first);
		}
		tidmap_iterator++;
	}
	for(unsigned int i=0;i<tid_to_be_deleted.size();i++){
		tidmap.erase(tid_to_be_deleted.at(i));
	}
	tidmapmutex.unlock();

	if(found==true){
		for(unsigned int i=0;i<tid_to_be_deleted.size();i++){
			this->putBackTID(tid_to_be_deleted.at(i));
		}
	}
#ifdef DEBUG
		coutmutex.lock();cout << "RMAPEngine::cancelRequestedCommand() completed" << endl;coutmutex.unlock();
#endif
}

void RMAPEngine::cancelAllRequestedCommands() throw (RMAPException){
	//not implemented yet
}

void RMAPEngine::send(vector<unsigned char>* packet){
	if(suspended==false){
		throw(RMAPException(RMAPException::RMAPEngine_MultipleTransactionScheme_NotSuspended));
	}
	sentpacketqueuemutex.lock();
	vector<unsigned char>* newpacket=new vector<unsigned char>;
		//this instance will be deleted in RMAPEngine_RequestProcessThread::run()
	(*newpacket)=(*packet); //copy byte-to-byte
	sentpacketqueue.push(newpacket);
	sentpacketqueuemutex.unlock();
	requestloopcondition.signal();
}

vector<unsigned char> RMAPEngine::receive(){
	while(true){
		//lock
		receivedpacketqueuemutex.lock();
		if(receivedpacketqueue.size()!=0){
			vector<unsigned char>* tmp=receivedpacketqueue.front();
			receivedpacketqueue.pop();
			vector<unsigned char> packet=*tmp; //copy byte-to-byte
			delete tmp;
			receivedpacketqueuemutex.unlock();
			return packet;
		}
		//unlock
		receivedpacketqueuemutex.unlock();
		//then wait meanwhile
		receiveloopcondition.wait(RMAPEngine::ReceiveWaitMilliSecond);
	}
}

void RMAPEngine::discardAllReceivedPackets(){
	receivedpacketqueuemutex.lock();
	while(receivedpacketqueue.size()!=0){
		receivedpacketqueue.pop();
	}
	receivedpacketqueuemutex.unlock();
}

void RMAPEngine::setSpaceWireIF(SpaceWireIF* newspacewireif){
	spacewireif=newspacewireif;
}

SpaceWireIF* RMAPEngine::getSpaceWireIF(){
	return spacewireif;
}

void RMAPEngine::enablePacketReceivingOutsideTIDScheme(){
	packetreceivingoutsidetidscheme=true;
}

void RMAPEngine::disablePacketReceivingOutsideTIDScheme(){
	packetreceivingoutsidetidscheme=false;
}

bool RMAPEngine::isPacketReceivingOutsideTIDSchemeEnabled(){
	return packetreceivingoutsidetidscheme;
}

void RMAPEngine::initializeLists(){
	for(unsigned int i=0;i<MaximumSocketNumber;i++){
		availablesocketids.push_back(MaximumSocketNumber-i);
	}
	usedsocketids.clear();
	for(unsigned int i=0;i<MaximumTID;i++){
		availabletids.push_back(i);
	}
#ifdef DEBUG
	cout << "RMAPEngine::initializeLists() *(availabletids.begin()) : " << *(availabletids.begin()) << endl;
#endif
	usedtids.clear();
}

unsigned int RMAPEngine::findAvailableSocketID() throw (RMAPException){
	if(availablesocketids.size()!=0){
		unsigned int socketid=*(availablesocketids.begin());
		availablesocketids.pop_front();
		usedsocketids.push_back(socketid);
		return socketid;
	}else{
		throw(RMAPException(RMAPException::TooManySockets));
	}
}

void RMAPEngine::putBackSocketID(unsigned int socketid){
	if (socketid<=MaximumSocketNumber) {
		usedsocketids.remove(socketid);
		availablesocketids.push_back(socketid);
	} else {
		throw(RMAPException(RMAPException::SocketIDOutOfRange));
	}
}

unsigned int RMAPEngine::findAvailableTID() throw (RMAPException){
	tidmapmutex.lock();
	if(availabletids.size()!=0){
		unsigned int tid=*(availabletids.begin());
		availabletids.pop_front();
		usedtids.push_back(tid);
		tidmapmutex.unlock();
#ifdef DEBUG
		coutmutex.lock();
		cout << "RMAPEngine::findAvailableTID() returning : " << tid << endl;
		cout << "RMAPEngine::findAvailableTID() availabletids.size() : " << availabletids.size() << endl;
		cout << "RMAPEngine::findAvailableTID() usedtids.size() : " << usedtids.size() << endl;
		coutmutex.unlock();
#endif
		return tid;
	}else{
		tidmapmutex.unlock();
		throw(RMAPException(RMAPException::TooManyRequests));
	}
}

void RMAPEngine::putBackTID(unsigned int tid){
	tidmapmutex.lock();
	if (tid<=MaximumTID) {
		usedtids.remove(tid);
		availabletids.push_back(tid);
		tidmapmutex.unlock();
	} else {
		tidmapmutex.unlock();
		throw(RMAPException(RMAPException::TIDOutOfRange));
	}
}

void RMAPEngine::registerSpaceWireExceptionHandler(SpaceWireExceptionHandler* handler){
	spacewireexceptionhandlerlist.push_back(handler);
}

void RMAPEngine::unregisterSpaceWireExceptionHandler(SpaceWireExceptionHandler* handler){
	try{
		spacewireexceptionhandlerlist.remove(handler);
	}catch(...){}
}

void RMAPEngine::handleSpaceWireException(SpaceWireException e){
	list<SpaceWireExceptionHandler*>::iterator it_begin=spacewireexceptionhandlerlist.begin();
	list<SpaceWireExceptionHandler*>::iterator it_end=spacewireexceptionhandlerlist.end();
	for(list<SpaceWireExceptionHandler*>::iterator it=it_begin;it!=it_end;it++){
		(*it)->handleSpaceWireException(e);
	}
}

void RMAPEngine::registerRMAPExceptionHandler(RMAPExceptionHandler* handler){
	rmapexceptionhandlerlist.push_back(handler);
}

void RMAPEngine::unregisterRMAPExceptionHandler(RMAPExceptionHandler* handler){
	try{
		rmapexceptionhandlerlist.remove(handler);
	}catch(...){}
}

void RMAPEngine::handleRMAPException(RMAPException e){
	list<RMAPExceptionHandler*>::iterator it_begin=rmapexceptionhandlerlist.begin();
	list<RMAPExceptionHandler*>::iterator it_end=rmapexceptionhandlerlist.end();
	for(list<RMAPExceptionHandler*>::iterator it=it_begin;it!=it_end;it++){
		(*it)->handleRMAPException(e);
	}
}



void RMAPEngine::dump(){
	cout << "===================================" << endl;
	cout << "RMAPEngine status dump        " << endl;
	cout << "-----------------------------------" << endl;
	cout << "Multiple Transaction Scheme : " << (!suspended ? "On":"Off") << endl;
	if(!suspended){
		cout << "# of Request         : " << requestmap.size() << endl;
		cout << "# of Reply Wait      : " << tidmap.size() << endl;
		cout << "# of Received Packet : " << receivedpacketqueue.size() << endl;
		cout << "# of Available TID   : " << availabletids.size() << endl;
		cout << "# of Used TID        : " << usedtids.size() << endl;
		cout << "-----------------------------------" << endl;
		cout << "Statistics                        " << endl;

		openclosemutex.lock();
		socketmapmutex.lock();
		map<unsigned int,RMAPSocket*>::iterator it;
		for(it=socketmap.begin();it!=socketmap.end();it++){
			cout << " Socket ID(" << it->first << ") Request Counter=" << it->second->getCounter() << endl;
		}
		socketmapmutex.unlock();
		openclosemutex.unlock();
	}else{
		cout << "Statistics                        " << endl;
		cout << "# of Packets to be Sent : " << sentpacketqueue.size() << endl;
		cout << "# of Packets Received : " << receivedpacketqueue.size() << endl;
	}
	cout << "===================================" << endl;
	cout << endl;
}

/*
 *
 */

RMAPEngine_RequestProcessThread::RMAPEngine_RequestProcessThread(
			SpaceWireIF* spwif, RMAPEngine* engine,Mutex* newtidmapmutex,Mutex* mutex,Condition* condition,
			map<unsigned int,unsigned int>* newtidmap,map<unsigned int,RMAPPacket*>* newrequestmap,
			map<unsigned int,RMAPDestination*>* newdestinationmap,Mutex* newdestinationmapmutex,
			queue<vector<unsigned char>*>* newsentpacketqueue,Mutex* newsentpacketqueuemutex,
			Mutex* newcoutmutex, Mutex* startstopmutex_send)
			: Thread(),spacewireif(spwif),rmapengine(engine),tidmapmutex(newtidmapmutex),
			requestmapmutex(mutex),requestloopcondition(condition),
			tidmap(newtidmap),requestmap(newrequestmap),
			destinationmap(newdestinationmap),destinationmapmutex(newdestinationmapmutex),
			sentpacketqueue(newsentpacketqueue),sentpacketqueuemutex(newsentpacketqueuemutex),
			coutmutex(newcoutmutex),startstopmutex_send(startstopmutex_send) {}

void RMAPEngine_RequestProcessThread::run() {
#ifdef DEBUG
	coutmutex->lock();cout << "RMAPEngine_RequestProcessThread::run() invoked" << endl;coutmutex->unlock();
#endif
	while (running) {
		requestloopcondition->wait(RMAPEngine::RequestProcessThreadWaitMilliSecond);
		//if Transaction ID control scheme is activated
		if(!suspended){
			requestmapmutex->lock();
			map<unsigned int, RMAPPacket*>::iterator i_begin=requestmap->begin();
			map<unsigned int, RMAPPacket*>::iterator i_end=requestmap->end();
#ifdef DEBUG
			if(i_begin!=i_end){
				coutmutex->lock();cout << "RMAPEngine_RequestProcessThread::run() sending" << endl;coutmutex->unlock();
			}
#endif
			for(map<unsigned int, RMAPPacket*>::iterator i=i_begin;i!=i_end;i++){
				unsigned int socketid=i->first;
				RMAPPacket* packet=i->second;
				unsigned int tid=rmapengine->findAvailableTID();
				packet->setTransactionID(tid);
#ifdef DEBUG
				coutmutex->lock();cout << "TID:" << tid << " SocketID:" << socketid << endl;cout.flush();coutmutex->unlock();
#endif
				if ( 0 != packet->getAckMode() ) {
					tidmapmutex->lock();
					tidmap->insert(pair<unsigned int,unsigned int>(tid, socketid));
					tidmapmutex->unlock();
				} else {
					rmapengine->putBackTID(tid);
				}
#ifdef DEBUG
				coutmutex->lock();cout << "RMAPEngine_RequestProcessThread::run() tid " << tid << " added" << endl;coutmutex->unlock();
#endif
				RMAPDestination* rmapdestination=(destinationmap->find(socketid))->second;
				packet->setRMAPDestination(*rmapdestination);
				packet->createPacket();
#ifdef DEBUG
				coutmutex->lock();
				cout << "RMAPEngine_RequestProcessThread::run() spacewireif->send()" << endl;
#ifdef DUMP
				SpaceWireUtilities::dumpPacket(packet->getPacket());
#endif
				coutmutex->unlock();
#endif
				try{
					spacewireif->send(packet->getPacket());
				}catch(SpaceWireException e){
#ifdef DEBUG
					coutmutex->lock();
					cout << "RMAPEngine_RequestProcessThread::run() spacewireif->send() exception" << endl;
					coutmutex->unlock();
#endif
					rmapengine->handleSpaceWireException(e);
				}
			}
			requestmap->clear();
			requestmapmutex->unlock();
		}else if(suspended){
			//if Transaction ID control scheme is suspended
			sentpacketqueuemutex->lock();
			if(!sentpacketqueue->empty()){
				while(!sentpacketqueue->empty()){
					vector<unsigned char>* packet=sentpacketqueue->front();
					try{
						spacewireif->send(packet);
					}catch(SpaceWireException e){
#ifdef DEBUG
						coutmutex->lock();cout << "RMAPEngine_RequestProcessThread::run() spacewireif->send() exception" << endl;coutmutex->unlock();
#endif
						rmapengine->handleSpaceWireException(e);
					}
					sentpacketqueue->pop();
					delete packet;
				}
			}
			sentpacketqueuemutex->unlock();
		}
	}
	startstopmutex_send->lock();
	startstopmutex_send->unlock();
}

RMAPEngine_ReplyProcessThread::RMAPEngine_ReplyProcessThread(
		SpaceWireIF* spwif, RMAPEngine* engine,Mutex* newtidmapmutex,Mutex* mutex,Condition* condition,
		map<unsigned int,unsigned int>* newtidmap,map<unsigned int,RMAPPacket*>* newreplymap,
		map<unsigned int,RMAPDestination*>* newdestinationmap,Mutex* newdestinationmapmutex,
		queue<vector<unsigned char>*>* newreceivedpacketqueue,Mutex* newreceivedpacketqueuemutex,
		Mutex* newcoutmutex, Mutex* startstopmutex_receive)
		: Thread(),spacewireif(spwif),rmapengine(engine),tidmapmutex(newtidmapmutex),
		replymapmutex(mutex),replyloopcondition(condition),
		tidmap(newtidmap),replymap(newreplymap),
		destinationmap(newdestinationmap),destinationmapmutex(newdestinationmapmutex),
		receivedpacketqueue(newreceivedpacketqueue),receivedpacketqueuemutex(newreceivedpacketqueuemutex),
		coutmutex(newcoutmutex),startstopmutex_receive(startstopmutex_receive) {}

void RMAPEngine_ReplyProcessThread::run() {

#ifdef DEBUG
	coutmutex->lock();cout << "RMAPEngine_ReplyProcessThread::run() invoked" << endl;coutmutex->unlock();
#endif
	runloop:
	while(running){
		RMAPPacket* packet=new RMAPPacket();
		vector<unsigned char>* receiveddata=new vector<unsigned char>;
		try{
			//receiveddata=new vector<unsigned char>;
			receiveloop:
#ifdef DEBUG
			coutmutex->lock();cout << "RMAPEngine_ReplyProcessThread::run() waiting packet" << endl;coutmutex->unlock();
#endif
			spacewireif->receive(receiveddata);
#ifdef DEBUG
			coutmutex->lock();cout << "RMAPEngine_ReplyProcessThread::run() received " << dec << receiveddata->size() << "bytes" << endl;coutmutex->unlock();
#endif
			if(receiveddata->size()==0){
				goto receiveloop;
			}
#ifdef DEBUG
			coutmutex->lock();cout << "RMAPEngine_ReplyProcessThread::run() : packet received" << endl;
#ifdef DUMP
			SpaceWireUtilities::dumpPacket(receiveddata);
#endif
			coutmutex->unlock();
#endif
		}catch(SpaceWireException e) {
#ifdef DUMP
			coutmutex->lock();cout << "RMAPEngine_ReplyProcessThread::run() Receive SpaceWireException" << endl;coutmutex->unlock();
#endif
			delete packet;
			delete receiveddata;
#ifdef DUMP
			coutmutex->lock();cout << "RMAPEngine_ReplyProcessThread::run() Receive SpaceWireException delete packet,receiveddata" << endl;coutmutex->unlock();
#endif
			rmapengine->handleSpaceWireException(e);
			goto runloop;
		}
		try{
			//lock tidmap
			tidmapmutex->lock();
#ifdef DEBUG
			coutmutex->lock();cout << "RMAPEngine_ReplyProcessThread::run() setPacket then interpretPacket" << endl;coutmutex->unlock();
#endif
			if(rmapengine->isSuspended()){
				throw RMAPException(RMAPException::NotAnRMAPPacket);
			}else{
				packet->setPacketBuffer(receiveddata);
			}
#ifdef DEBUG
			coutmutex->lock();cout << "RMAPEngine_ReplyProcessThread::run() setPacket then interpretPacket done" << endl;coutmutex->unlock();
#endif
			//set received data to RMAPPacket and interpret (automatically performed in setPacket())
			//this method may throw RMAPException if the packet is not a correct RMAP packet.
			//packet->setPacketBuffer(receiveddata);
#ifdef DEBUG
			coutmutex->lock();cout << "RMAPEngine_ReplyProcessThread::run() : received packet TID = " << packet->getTransactionID() << endl;coutmutex->unlock();
#endif
			unsigned int tid=packet->getTransactionID();
			//judgement flag
			bool receivedPacketHasCorrespondingSocket=false;

			//find a socketid which corresponds to the received Transaction ID
			map<unsigned int,unsigned int>::iterator tidmap_find=tidmap->find(tid);
			map<unsigned int,unsigned int>::iterator tidmap_end=tidmap->end();

			//find a destination which corresponds to the received Transaction ID
			if(tidmap_find!=tidmap_end){
				unsigned int socketid=tidmap_find->second;
				map<unsigned int,RMAPDestination*>::iterator destinationmap_find=destinationmap->find(socketid);
				map<unsigned int,RMAPDestination*>::iterator destinationmap_end=destinationmap->end();
				if(destinationmap_find!=destinationmap_end){
#ifdef DEBUG
					coutmutex->lock();cout << "RMAPEngine_ReplyProcessThread::run() : received destination info found in destinationmap" << endl;coutmutex->unlock();
#endif
					RMAPDestination* destination=destinationmap_find->second;
					if(packet->getRMAPDestination()->equals(destination)){
#ifdef DEBUG
						coutmutex->lock();cout << "RMAPEngine_ReplyProcessThread::run() : found corresponding socket" << endl;coutmutex->unlock();
#endif
						packet->setRMAPDestination(*destination);
						receivedPacketHasCorrespondingSocket=true;
					}else{
#ifdef DEBUG
						coutmutex->lock();
						cout << "RMAPEngine_ReplyProcessThread::run() : Destination differs" << endl;
#ifdef DUMP
						cout << "Registered RMAPDestination : " << endl;
						destination->dump();
						cout << "Received RMAPDestination : " << endl;
						packet->getRMAPDestination()->dump();
#endif
						coutmutex->unlock();
#endif
					}
				}
			}

			//if tid is in use, and the destination is the same as expected from the tid
			if(receivedPacketHasCorrespondingSocket==true && suspended==false){
				//found
				unsigned int socketid=tidmap_find->second;
				//add entry [socketid,RMAPPacket*] to replymap
				replymapmutex->lock();
				replymap->insert(pair<unsigned int,RMAPPacket*>(socketid,packet));
				replymapmutex->unlock();
				//delete  entry [tid,scoketid] from tidmap
				tidmap->erase(tidmap_find);
				tidmapmutex->unlock();
				rmapengine->putBackTID(tid);
#ifdef DEBUG
				coutmutex->lock();cout << "Packet received TID : " << tid << " Socket ID : " << socketid << endl;coutmutex->unlock();
#endif
			}else{
#ifdef DEBUG
				coutmutex->lock();cout << "RMAPEngine_ReplyProcessThread::run() : corresponding socket NOT found" << endl;coutmutex->unlock();
#endif
				tidmapmutex->unlock();
				//corresponding socket not found
				//in this case, put the received packet to temporal "Received Packet Queue"
#ifdef DEBUG
				coutmutex->lock();cout << "Packet received TID : " << tid << " and pushed into receivedpacketqueue" << endl;coutmutex->unlock();
#endif
				if(rmapengine->isPacketReceivingOutsideTIDSchemeEnabled()){
					receivedpacketqueuemutex->lock();
					receivedpacketqueue->push(receiveddata);
					receivedpacketqueuemutex->unlock();
				}
				delete packet;
			}
		}catch(RMAPException e){
			tidmapmutex->unlock();
#ifdef DEBUG
			e.dump();
			coutmutex->lock();cout << "Packet received and pushed into receivedpacketqueue" << endl;coutmutex->unlock();
#endif
			if(rmapengine->isPacketReceivingOutsideTIDSchemeEnabled()){
				receivedpacketqueuemutex->lock();
				vector<unsigned char>* copiedreceiveddata=new vector<unsigned char>();
				*copiedreceiveddata=*receiveddata;
				receivedpacketqueue->push(copiedreceiveddata);
				receivedpacketqueuemutex->unlock();
			}
			delete packet;
		}
#ifdef DEBUG
		coutmutex->lock();cout << "RMAPEngine_ReplyProcessThread::run() : broadcasts 'Packet Received' signal to receive-waiting threads" << endl;coutmutex->unlock();
#endif
		replyloopcondition->broadcast();
	}
	startstopmutex_receive->lock();
	startstopmutex_receive->unlock();
}

/** History
 * 2008-05-xx file created (Takayuki Yuasa)
 * 2008-12-13 delete packet in "not an RMAP packet" receiving process (Takayuki Yuasa)
 * 2008-12-16 added SpaceWireExceptionHandler (Takayuki Yuasa)
 */
