#ifndef SPACEWIREASYNCHRONOUSSTATUS_HH_
#define SPACEWIREASYNCHRONOUSSTATUS_HH_

#include "SpaceWireException.hh"
#include "SpaceWireLinkStatus.hh"

#include <map>
using namespace std;

/** This class holds information which is needed for
 * users to control an asynchronous operations in SpaceWire 
 * layer. For example, when a user want to wait for a packet
 * which may be sent from other nodes in background whereas
 * want to continue other tasks, he/she can start asynchronous
 * receive operation by calling asynchronousReceive() method
 * of SpaceWireIF class. The method provides an instance of
 * SpaceWireAsynchronousStatus class which holds status of
 * the asynchronous (background) receive operation, such as
 * operation number (an ID which is required to confirm the
 * completion of the process).
 * The implementation of this class totally depends on the
 * functionalitiy of the driver or lower layer library APIs
 * provided to use SpaceWire I/F.
 */
class SpaceWireAsynchronousStatus {
private:
	int asynchronousOperationID;
	SpaceWireLinkStatus linkstatus;
	static int CurrentAsynchronousProcessID;
	static map<int,bool> processIDMap;
public:
	/** Constructor.
	 */
	SpaceWireAsynchronousStatus();
		
	/** Destructor.
	 */
	virtual ~SpaceWireAsynchronousStatus();
	
	/** Getter for asynchronous process id.
	 * @return asynchronous process id
	 */
	virtual int getID();
	
	/** Setter for SpaceWirelinkstatus.
	 * @param newlinkstatus link status
	 */
	virtual void setLinkStatus(SpaceWireLinkStatus newlinkstatus);
	
	/** Getter for SpaceWirelinkstatus.
	 * @return link status
	 */
	virtual SpaceWireLinkStatus getLinkStatus();
	
	/** Waits asynchronous process of which information
	 * this class holds. This method should be override
	 * using system calls of host APIs. 
	 */
	virtual void waitCompletion() throw(SpaceWireException);
	
	/** Increments AsynchronousProcessID for new instance.
	 * This method returns new AsynchronousProcessID. 
	 * This method adds the ID to a map to manage the wait status.
	 * This method also controls max,min of AsynchronousProcessID.
	 * @return asynchronous process id
	 */
	int newAsynchronousProcessID();
	
	/** Clear AsynchronousProcessID for next use.
	 * This method actually clear internal map between
	 * AsynchronousProcessID and wait status.
	 * This method should be called after completion of
	 * the asynchronous process managed with the AsynchronousProcessID.
	 * @param id asynchronous process id
	 */
	void clearAsynchronousProcessID(int id);
	
public:
	static const int DefaultAsynchronousProcessID=-100;
	static const int MinimumAsynchronousProcessID=0;
	static const int MaximumAsynchronousProcessID=1000;
};

#endif /*SPACEWIREASYNCHRONOUSSTATUS_HH_*/
