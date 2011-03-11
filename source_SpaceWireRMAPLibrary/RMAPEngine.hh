#ifndef RMAPENGINE_HH_
#define RMAPENGINE_HH_

#include "RMAPDestination.hh"
#include "RMAPExceptionHandler.hh"
#include "RMAPPacket.hh"
#include "RMAPSocket.hh"
#include "SpaceWireIF.hh"

#include "Mutex.hh"
#include "Condition.hh"
#include "Thread.hh"

#include <list>
#include <map>
#include <queue>
#include <string>
#include <vector>
using namespace std;

class RMAPSocket; //dummy
class RMAPEngine_RequestProcessThread; //dummy
class RMAPEngine_ReplyProcessThread; //dummy


/** A class which controls simultaneous send/receive
 * processes via SpaceWireIF, and operates multiple
 * transaction. In this class, two concurrent threads
 * are started to control send/receive processes.
 */
//class RMAPEngine : public RMAPExceptionHandler, public SpaceWireExceptionHandler {
class RMAPEngine : public RMAPExceptionHandler {
private:
	/** Reference to SpaceWireIF used in this class.
	 */
	SpaceWireIF* spacewireif;

	/** A map which holds the relation between
	 * Transaction ID and Socket ID.
	 */
	map<unsigned int,unsigned int> tidmap; //TID,socketid

	/** A map which holds the relation between
	 * Socket ID and send-requested RMAPPacket.
	 * The Transaction ID of the RMAPPacket is newly allocated
	 * when it is actually sent out from this RMAPEngine.
	 * The allocated Transaction ID is stored to tidmap.
	 */
	map<unsigned int,RMAPPacket*> requestmap; //socketid,requested packet

	/** A map which holds the relation between
	 * Socket ID and replied (received) RMAPPacket.
	 * The Transaction ID of the RMAPPacket is interpreted,
	 * and then a Socket ID which corresponds to the Transaction
	 * ID is searched from tidmap. The RMAPPacket is stored
	 * to this replymap with the searched Socket ID so that
	 * a socket which requested the RMAP access can retrieve
	 * the replied packet.
	 */
	map<unsigned int,RMAPPacket*> replymap; //socketid,replied packet

	/** A map which hold the relation between
	 * Socket ID and RMAPDestination.
	 */
	map<unsigned int,RMAPDestination*> destinationmap; //socketid,RMAPDestination*

	/** A map which hold the relation between
	 * Socket ID and reference to RMAPSocket.
	 */
	map<unsigned int,RMAPSocket*> socketmap; //socketid,RMAPSocket*

	/** A queue for sent packets that should be transferred without
	 * Transaction ID control scheme.
	 */
	queue<vector<unsigned char>*> sentpacketqueue;

	/** A queue for received packets that could not be
	 * delivered to exisiting sockets or received when Transaction
	 * ID control scheme is disabeld.
	 */
	queue<vector<unsigned char>*> receivedpacketqueue;

	/** A list which holds available Socket IDs.
	 * When a new socket is opened, RMAPEngine retrieve
	 * a Socket ID from this list by calling findAvailableTID(),
	 * and allocates it to the opened Socket.
	 */
	list<unsigned int> availablesocketids;

	/** A list which holds Socket IDs in use.
	 */
	list<unsigned int> usedsocketids;

	/** A list which holds available Transaction IDs.
	 * When a new RMAP transaction is started, RMAPEngine retrieve
	 * a Transaction ID from this list by calling
	 * findAvailableSocketID(), and allocates it to
	 * the started transaction.
	 */
	list<unsigned int> availabletids;

	/** A list which holdds Transaction IDs in use.
	 */
	list<unsigned int> usedtids;

	Mutex startstopmutex_send;
	Mutex startstopmutex_receive;
private:
	/** An internal thread which is responsible for packet sending.
	 */
	RMAPEngine_RequestProcessThread* requestprocessthread;

	/** An internal thread which is responsible for packet receiving.
	 */
	RMAPEngine_ReplyProcessThread* replyprocessthread;
private:
	/** Exception handler for SpaceWire layer.
	 */
	list<SpaceWireExceptionHandler*> spacewireexceptionhandlerlist;

	/** Exception handler for RMAP layer.
	 */
	list<RMAPExceptionHandler*> rmapexceptionhandlerlist;

public:
	/** Mutex and Conditions used to synchronize multiple thread.
	 */
	Mutex tidmapmutex;
	Mutex requestmapmutex;
	Mutex replymapmutex;
	Condition requestloopcondition;
	Condition replyloopcondition;
	Condition receiveloopcondition;
	Mutex coutcinmutex;
	Mutex openclosemutex;
	Mutex sentpacketqueuemutex;
	Mutex receivedpacketqueuemutex;
	Mutex destinationmapmutex;
	Mutex socketmapmutex;
	Mutex coutmutex;

private:
	/** Flag which is used to judge the state of
	 * this RMAPEngine Transaction ID Control scheme.
	 */
	bool suspended;

	/** Flag which is used to determine whether
	 * this RMAPEngine can receive packets that
	 * are not expected to be delivered.
	 * The received (unexpected) packets are
	 * pushed into "receivedpacketqueue".
	 */
	bool packetreceivingoutsidetidscheme;

public:
	/** Constructor.
	 */
	RMAPEngine();

	/** Constructor with SpaceWireIF instance.
	 * @param newspacewireif a reference to SpaceWireIF used in this RMAPEngine class
	 */
	RMAPEngine(SpaceWireIF* newspacewireif);

	/** Destructor.
	 */
	virtual ~RMAPEngine();
private:
	/** An internal method for intialization.
	 */
	void initialize();

	/** An internal method for intialization of
	 * multiple lists (Socket ID list, Transaction ID list).
	 */
	void initializeLists();
public:
	/** Starts the send/receive threads so that sending and receiving
	 * packets can be simultaneously performed. Users must invoke
	 * this method before opening RMAPSocket with openRMAPSocketTo()
	 * method. After calling this method, users cannot use SpaceWireIF
	 * instance directly because it is controlled by RMAPEngine's internal
	 * threads.
	 */
	void start();

	/** Stops the send/receive threads. Before deleting the RMAPEngine
	 * instance, users should invoke this method so that these threads
	 * are correctly stopped. After calling this method, SpaceWireIF can
	 * be used by users directrly.
	 */
	void stop();

	/** Suspend the Transaction ID control scheme. By calling this
	 * method, RMAPEngine stops the scheme, and users can perform
	 * arbitrary send/receive operation via sendPacket() and
	 * receivePacket() methods. Warning : Simultaneous multiple
	 * transaction via RMAPSocket stops working.
	 */
	void suspend();

	/** Check if this RMAPEngine is suspended.
	 */
	inline bool isSuspended(){
		return suspended;
	}
	/** The same method as suspend() except that this method will
	 *  only be invoked by child threads of RMAPEngine (i.e.
	 * RMAPEngine_RequestProcessThread and RMAPEngine_ReplyProcessThread).
	 */
//	void suspendBy();

	/** Resumes the Transaction ID control shceme.
	 */
	void resume();

public:
	/** Open new RMAPSocket which is connected to the ordered
	 * RMAPDestination. RMAPPacket is created/interpreted
	 * automatically in the RMAPSocketUsers can performe RMAP
	 * Read/Write from/to the RMAPDestination using read()/write()
	 * method of this RMAPSocket.
	 * @param newdestination destination of this RMAPSocket
	 * @return a reference to created RMAPSocket instance
	 */
	RMAPSocket* openRMAPSocketTo(RMAPDestination newdestination) throw (RMAPException);

	/** Close RMAPSocket given as an argument.
	 *
	 */
	void closeRMAPSocket(RMAPSocket* socket);
public:
	/** Register a request of sending RMAP Command packet. This method is
	 * usually called by RMAPSocket. This method tells the request to
	 * RMAPEngine_RequestProcessThread class by calling signal() of
	 * a Condition instance (requestloopcondition) to wake up
	 * RMAPEngine_RequestProcessThread class which is waiting a new
	 * send request.
	 * @param socket a pointer to RMAPSocket which calls this method
	 * @param commandpacket a reference to an RMAPPacket which contains information of the requested RMAP Command
	 */
	void requestCommand(RMAPSocket* socket,RMAPPacket* commandpacket) throw (RMAPException);

	/** (Old version. Not recommended to use)
	 * Register a request of sending RMAP Command packet. This method is
	 * usually called by RMAPSocket. This method tells the request to
	 * RMAPEngine_RequestProcessThread class by calling signal() of
	 * a Condition instance (requestloopcondition) to wake up
	 * RMAPEngine_RequestProcessThread class which is waiting a new
	 * send request.
	 * @param socketid id of socket which ask this send requeset
	 * @param commandpacket a reference to an RMAPPacket which contains information of the requested RMAP Command
	 */
	void requestCommand(unsigned int socketid,RMAPPacket* commandpacket) throw (RMAPException);

	/** Register a request of receiving RMAP Reply packet. This method is
	 * usually called by RMAPSocket. This method waits for the receiption
	 * of Reply Packet using wait() of a Condition instance
	 * (replloopcondition).
	 * RMAPEngine_ReplyProcessThread can tell the reception of Reply Packet
	 * by calling Condition::signal().
	 * @param socket a pointer to RMAPSocket which calls this method
	 * @return a pointer to the received RMAP Reply Packet
	 */
	RMAPPacket* receiveReply(RMAPSocket* socket) throw (RMAPException);

	/** (Old version. Not recommended to use)
	 * Register a request of receiving RMAP Reply packet. This method is
	 * usually called by RMAPSocket. This method waits for the receiption
	 * of Reply Packet using wait() of a Condition instance
	 * (replloopcondition).
	 * RMAPEngine_ReplyProcessThread can tell the reception of Reply Packet
	 * by calling Condition::signal().
	 * @param socketid id of socket which ask this receive request
	 * @return received RMAP Reply Packet
	 */
	RMAPPacket receiveReply(unsigned int socketid) throw (RMAPException);

	/** Cancel the request issued from a certain socket.
	 * This method delete the request from request queue. If the request
	 * has been already sent to the target node, the method also deletes
	 * the corresponding Transaction ID from tidmap, and by doing so,
	 * it cancels the reply-waiting for the Transaction ID in
	 * RMAPEngine_ReplyProcessThread class.
	 *
	 * Warning : a race condition can appear if the reply packet arrives
	 * soon after the deletion of tidmap entry. In that case, users
	 * should retrieve the arrived packet (which is stored in receivedpacketqueue)
	 * by receive() method and discard it.
	 *
	 * @param the id of socket whose request is about to be canceled
	 */
	void cancelRequestedCommand(unsigned int socketid) throw (RMAPException);

	/** Cancel all the issued requests.
	 */
	void cancelAllRequestedCommands() throw (RMAPException);

public:
	/** Send a packet without using Transaction ID control scheme of
	 * RMAPEngine. The given RMAPPacket is sent via SpaceWireIF without
	 * any modification, i.e. the Transaction ID which is designated in
	 * the RMAPPacket is used. Before invoking this method, users
	 * should suspend the Transaction ID control scheme of RMAPEngine
	 * by calling suspend().
	 */
	 void send(vector<unsigned char>* data);

	/** Retrive a received packet from a queue which holds
	 * packets that could not be delivered to exisiting sockets.
	 * @return received RMAP Packet
	 */
	 vector<unsigned char> receive();

	 /** Discards all packets that could not be delivered to exisiting
	  * sockets and stored in "received packet queue".
	  */
	 void discardAllReceivedPackets();

public:
	/** Setter for reference to SpaceWireIF which is used in
	 * this RMAPEngine class.
	 * @param newspacewireif a reference to SpaceWireIF used in this RMAPEngine class
	 */
	void setSpaceWireIF(SpaceWireIF* newspacewireif);

	/** Getter for reference to SpaceWireIF which is used in
	 * this RMAPEngine class.
	 * @return a reference to SpaceWireIF used in this RMAPEngine class
	 */
	SpaceWireIF* getSpaceWireIF();
public:
	/** Enable receiving of packets which do not fall in the Transaction ID
	 * control scheme. By enabling it, users can retrieve packets from
	 * "received packet queue" by invoking receive() method.
	 */
	void enablePacketReceivingOutsideTIDScheme();

	/** Disable receiving of packets which do not fall in the Transaction ID
	 * control scheme. By enabling it, all received packets except for
	 * right Reply packet will be discarded automatically.
	 */
	void disablePacketReceivingOutsideTIDScheme();

	/** Getter for the mode of packet receiving outside TID control scheme.
	 */
	bool isPacketReceivingOutsideTIDSchemeEnabled();

private:
	/** A method which retrieve an available Socket ID.
	 * Retrieved Socket ID is allocated to a newly opened
	 * socket.
	 * @return socket id
	 */
	unsigned int findAvailableSocketID() throw (RMAPException);

	/** A method which put back a Socket ID to
	 * available state.
	 * @param socketid socket id to be restored to available state
	 */
	void putBackSocketID(unsigned int socketid);
public:
	/** A method which retrieve an available Transaction ID.
	 * Retrieved Transaction ID is allocated to a newly
	 * started transaction.
	 * @return transaction id
	 */
	unsigned int findAvailableTID() throw (RMAPException);

	/** A method which put back a Transaction ID to
	 * available state. This method should be called
	 * only by RMAPEngine class and RMAPEngine_ReplyProcessThread
	 * class.
	 * @param tid transaction id to be restored to available state
	 */
	void putBackTID(unsigned int tid);
public:
	/** Register a SpaceWireExceptionHander instance to
	 * SpaceWire-exception internal hander list. Registered handler
	 * instances are invoked in a sequential order when a SpaceWire-
	 * Exception is thrown in SpaceWire-related operations.
	 * @param handler SpaceWireExceptionHandler instance to be registered
	 */
	void registerSpaceWireExceptionHandler(SpaceWireExceptionHandler* handler);

	/** Get rid of the registered exception handler from
	 * SpaceWire-exception handler list.
	 * @param handler RMAPExceptionHandler instance to be unregistered
	 */
	void unregisterSpaceWireExceptionHandler(SpaceWireExceptionHandler* handler);


	/** Handler for SpaceWireException. This method executes registered
	 * SpaceWireExceptionHandler's handleSpaceWireException() method in a
	 * sequential order.
	 * @param e exception
	 */
	void handleSpaceWireException(SpaceWireException e);
public:
	/** Register an RMAPExceptionHandler instance to internal
	 * RMAP-exception handler list. Registered handler instances
	 * are invoked in a sequential order when an RMAPException
	 * is thrown in SpaceWire or RMAP operations.
	 * @param handler RMAPExceptionHandler instance to be registered
	 */
	void registerRMAPExceptionHandler(RMAPExceptionHandler* handler);

	/** Get rid of the registered exception handler from
	 * RMAP-exception handler list.
	 * @param handler RMAPExceptionHandler instance to be unregistered
	 *
	 */
	void unregisterRMAPExceptionHandler(RMAPExceptionHandler* handler);

	/** Handler for RMAPException. This method executes registered
	 * RMAPExceptionHandler's handleRMAPException() method in a
	 * sequential order.
	 * @param e exception
	 */
	void handleRMAPException(RMAPException e);

public:
	/** Dump the status of RMAPEngine instance
	 */
	void dump();
public:
	/** Maximum number of simultaneously opened sockets.
	 */
	static const unsigned int MaximumSocketNumber=1024;

	/** Maximum number of simultaneous transactions.
	 */
	static const unsigned int MaximumTID=200;//65536;

	/** Default wait time out duration for receive process.
	 */
	static const unsigned int ReceiveWaitMilliSecond=10;

	/** Default wait time out duration for RequestProcess thread.
	 */
	static const unsigned int RequestProcessThreadWaitMilliSecond=10;

};

/** This class operates Packet sending as child thread of RMAPEngine.
 * Transaction ID control scheme is usually enabled to realize
 * simultaneous multiple transaction. Otherwise, when transferring
 * packets without Transaction ID re-allocation, the function should
 * be disabled by calling suspend() (this is automatically performed
 * in RMAPEngine's suspend() method).
 */
class RMAPEngine_RequestProcessThread : public Thread {
private:
	SpaceWireIF* spacewireif;
	RMAPEngine* rmapengine;
	Mutex* tidmapmutex;
	Mutex* requestmapmutex;
	Condition* requestloopcondition;
	map<unsigned int,unsigned int>* tidmap; //TID,socketid
	map<unsigned int,RMAPPacket*>* requestmap; //socketid,request status(true:requesting)
	map<unsigned int,RMAPDestination*>* destinationmap; //socketid,RMAPDestination*
	Mutex* destinationmapmutex;
	queue<vector<unsigned char>*>* sentpacketqueue;
	Mutex* sentpacketqueuemutex;
	Mutex* coutmutex;
	Mutex* startstopmutex_send;
	bool running;
	bool suspended;
public:
	RMAPEngine_RequestProcessThread(
			SpaceWireIF* spwif, RMAPEngine* engine,Mutex* newtidmapmutex,Mutex* mutex,Condition* condition,
			map<unsigned int,unsigned int>* newtidmap,map<unsigned int,RMAPPacket*>* newrequestmap,
			map<unsigned int,RMAPDestination*>* newdestinationmap,Mutex* newdestinationmapmutex,
			queue<vector<unsigned char>*>* newsentpacketqueue,Mutex* newsentpacketqueuemutex,
			Mutex* newcoutmutex,Mutex* startstopmutex_send);
	~RMAPEngine_RequestProcessThread(){}
	/** This method works as a main part of this class. It handles packet
	 * sending and calls SpaceWireIF::send(). It also controls the relationship
	 * between Request Socket IDs and Transaction IDs.
	 * If an exception occurs in SpaceWireIF layer, this method calls
	 * RMAPEngine::suspend() to suspend the functionalities of RMAPEngine and
	 * its child threads. After that, the exception is notified to user-layer
	 * by re-throwing. Users should make a choice between abandoning the
	 * RMAPEngine instance or resuming it again with new (i.e. re-opened and
	 * re-connected) SpaceWireIF instance.
	 */
	virtual void run();
	void start(){
		running=true;
		suspended=false;
		Thread::start();
	}
	void suspend(){
		suspended=true;
	}
	void resume(){
		suspended=false;
	}
	void stop(){running=false;}
};

/** This operates Packet receiving as child thread of RMAPEngine.
 * Transaction ID control scheme is usually enabled to realize
 * simultaneous multiple transaction. Otherwise, when transferring
 * packets without Transaction ID re-allocation, the function should
 * be disabled by calling suspend() (this is automatically performed
 * in RMAPEngine's suspend() method).
 */
class RMAPEngine_ReplyProcessThread : public Thread {
private:
	SpaceWireIF* spacewireif;
	RMAPEngine* rmapengine;
	Mutex* tidmapmutex;
	Mutex* replymapmutex;
	Condition* replyloopcondition;
	map<unsigned int,unsigned int>* tidmap; //TID,socketid
	map<unsigned int,RMAPPacket*>* replymap; //socketid,reply status(true:reply ready)
	map<unsigned int,RMAPDestination*>* destinationmap; //socketid,RMAPDestination*
	Mutex* destinationmapmutex;
	queue<vector<unsigned char>*>* receivedpacketqueue;
	Mutex* receivedpacketqueuemutex;
	Mutex* coutmutex;
	Mutex* startstopmutex_receive;
	bool running;
	bool suspended;
public:
	RMAPEngine_ReplyProcessThread(
			SpaceWireIF* spwif, RMAPEngine* engine,Mutex* newtidmapmutex,Mutex* mutex,Condition* condition,
			map<unsigned int,unsigned int>* newtidmap,map<unsigned int,RMAPPacket*>* newreplymap,
			map<unsigned int,RMAPDestination*>* newdestinationmap,Mutex* newdestinationmapmutex,
			queue<vector<unsigned char>*>* newreceivedpacketqueue,Mutex* newreceivedpacketqueuemutex,
			Mutex* newcoutmutex, Mutex* startstopmutex_receive);
	~RMAPEngine_ReplyProcessThread(){}
	/** This method works as a main part of this class. It handles packet
	 * receiving and calls SpaceWireIF::receive().
	 * If an exception occurs in SpaceWireIF layer, this method calls
	 * RMAPEngine::suspend() to suspend the functionalities of RMAPEngine and
	 * its child threads. After that, the exception is notified to user-layer
	 * by re-throwing. Users should make a choice between abandoning the
	 * RMAPEngine instance or resuming it again with new (i.e. re-opened and
	 * re-connected) SpaceWireIF instance.
	 */
	virtual void run();
	void start(){
		running=true;
		suspended=false;
		Thread::start();
	}
	void suspend(){
		suspended=true;
	}
	void resume(){
		suspended=false;
	}
	void stop(){running=false;}
};

#endif /*RMAPENGINE_HH_*/

/** History
 * 2008-05-xx file created (Takayuki Yuasa)
 */
