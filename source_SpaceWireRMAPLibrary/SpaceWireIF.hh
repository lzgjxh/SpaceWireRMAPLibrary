#ifndef SPACEWIREIF_HH_
#define SPACEWIREIF_HH_

#include "SpaceWireException.hh"
#include "SpaceWireLinkStatus.hh"
#include "SpaceWireAsynchronousStatus.hh"

#include "Mutex.hh"

#include <vector>
#include <iostream>
using namespace std;

/** An abstract class which includes a method invoked when
 * TimeCode is received.
 */
class SpaceWireTimeCodeAction {
public:
	SpaceWireTimeCodeAction(void) {}
	virtual ~SpaceWireTimeCodeAction(void) {}
	virtual void doAction(unsigned char timecode)=0;
};


/** An abstract class for encapsulation of a SpaceWire interface.
 *  This class provides virtual methods for opening/closing the interface
 *  and sending/receiving a packet via the interface.
 *  Physical or Virtual SpaceWire interface class should be created
 *  by inheriting this class.
 */
class SpaceWireIF {
protected:
	/** The index of this SpaceWire port.
	 */
	unsigned int portnumber;
	SpaceWireLinkStatus linkstatus;
	static bool initialized;
	Mutex initializationmutex;
	bool debugmode;
	SpaceWireTimeCodeAction* timecodeaction;
public:
	/** Constructor.
	 */
	SpaceWireIF();

	/** Destructor.
	 */
	virtual ~SpaceWireIF();

	/** A virtual method for initializing a SpaceWire interface.
	 * This method should be called in advance of other methods
	 * in the instance, and should not be invoked more than once.
	 * @return result of initializatin
	 */
	virtual bool initialize() throw(SpaceWireException) =0;

	/** A pure virtual method for opening a SpaceWire interface.
	 * In this method, port number is automatically set (depends on
	 * the implementation).
	 * The device driver is opened in this method. After the invokation
	 * of this method, the SpaceWire interface will start an initialization
	 * sequence (send NULL code to the other side node).
	 * @return result of open operation
	 */
	virtual bool open() throw(SpaceWireException) =0;

	/** A pure virtual method for opening a SpaceWire interface with
	 * specific port number "N". The device driver for the designated port
	 * is opened in this method. After the invokation of this method,
	 * the SpaceWire interface (port "N") will start an initialization
	 * sequence (send NULL code to the other side node).
	 * @return result of open operation
	 */
	virtual bool open(int portnumber) throw(SpaceWireException) =0;

	/** A pure virtual method for closing a SpaceWire interface.
	 */
	virtual void close() =0;

	/** A pure virtual method for starting a link intialization
	 * process by sending a NULL to the opposite node.
	 */
	virtual void startLinkInitializationSequenceAsAMasterNode() throw(SpaceWireException) =0;

	/** A pure virtual method for starting a link initialization
	 * after receiving a NULL from the opposite node.
	 */
	virtual void startLinkInitializationSequenceAsASlaveNode() throw(SpaceWireException) =0;

	/** A pure virtual method for closing a connection.
	 */
	virtual void stopLink() throw(SpaceWireException) =0;

	/** Get link status.
	 * @return link status
	 */
	virtual SpaceWireLinkStatus getLinkStatus() throw(SpaceWireException);

	/** Set link status (especially link speed and timeout duration).
	 * @param newstatus link status
	*/
	virtual void setLinkStatus(SpaceWireLinkStatus newstatus) throw(SpaceWireException);

	/** A pure virtual method for resetting the SpaceWire State
	 * Machine in the SpaceWire I/F logic.
	 */
	virtual void resetStateMachine() throw(SpaceWireException) =0;

	/** A pure virtual method for resetting the send FIFO of
	 * the SpaceWire I/F.
	 */
	virtual void clearSendFIFO() throw(SpaceWireException) =0;

	/** A pure virtual method for resetting the receive FIFO of
	 * the SpaceWire I/F.
	 */
	virtual void clearReceiveFIFO() throw(SpaceWireException) =0;

	/** A pure virtual method for reseting a SpaceWire interface.
	 * Degree of the reset action depends on an implementation.
	 * For example, library software and the interface hardware (FPGA or ASIC, etc)
	 * may be reset, whereas the hardware may not in other implementation.
	 */
	virtual bool reset() throw(SpaceWireException) =0;

	/** A pure virtual method for finalizing a SpaceWire interface.
	 * This method should be called before the instance will be deleted.
	 * This method frees buffers, instances, etc. which were created in
	 * the class instance.
	 */
	virtual void finalize() throw(SpaceWireException) =0;

	/** A pure virtual method for sending a packet. This method will return after
	 * completion of the packet sending or throws an exception on the
	 * way of the sending if there occurs an error.
	 * @param packet a reference to a sent data vector
	 */
	virtual void send(vector<unsigned char>* packet) throw(SpaceWireException) =0;

	/** A pure virtual method for receiving a packet. This method will return after
	 * completion of the packet receiving or throws an exception on the
	 * way of the sending if there occurs an error.
	 * @return received data vector
	 */
	virtual vector<unsigned char> receive() throw(SpaceWireException) =0;

	/** A pure virtual method for receiving a packet. This method will return after
	 * completion of the packet receiving or throws an exception on the
	 * way of the sending if there occurs an error. The received packet will be
	 * stored in the buffer specified as an argument.
	 * @param packet vector buffer in which received packet will be stored
	 */
	virtual void receive(vector<unsigned char>* packet) throw(SpaceWireException) =0;

	/** A virtual method for receiving a packet.
	 * An array should be passed to return a received packet data.
	 */
	virtual void receive(unsigned char* array,unsigned int* receivedsize){};

	/** A pure virtual method for aborting packet transfer. This method can be called
	 * to abort waiting method such as receive().
	 */
	virtual void abort() throw(SpaceWireException) =0;

	/** A pure virtual method for receiving a packet. This method will return immediately
	 * after starting the receive process and the process status is stored in
	 * SpaceWireAsynchronousStatus. The completion of the packet receiving
	 * can be waited or checked by calling the method of the status class;
	 * waitAsynchronousReceive() or isAsynchronousReceiveCompleted(), respectively.
	 * @return asynchronous status class
	 */
	virtual SpaceWireAsynchronousStatus asynchronousReceive()
		throw(SpaceWireException) =0;

	/** A pure virtual method for sending a packet. This method will return immediately
	 * after starting the send process and the process status is stored in
	 * SpaceWireAsynchronousStatus. The completion of the packet sending
	 * can be waited or checked by calling the method of the status class;
	 * waitAsynchronousReceive() or isAsynchronousReceiveCompleted(), respectively.
	 * @param packet a reference to a sent data vector
	 * @return asynchronous status class
	 */
	virtual SpaceWireAsynchronousStatus asynchronousSend(vector<unsigned char>* pacekt)
		throw(SpaceWireException) =0;

	/** A pure virtual method for waiting the completion of an asynchronous operation.
	 * The target operation is directed as a SpaceWireAsynchronousStatus argument.
	 * @param status asynchronous status class
	 */
	virtual void waitCompletionOfAsynchronousOperation(SpaceWireAsynchronousStatus status)
		throw(SpaceWireException) =0;
public:
	/** A pure virtual method for sending TimeCode.
	 * @param flag_and_timecode 2bit Flag + 6bit TimeCode
	 */
	virtual void sendTimeCode(unsigned char flag_and_timecode) throw(SpaceWireException) =0;

	/** A pure virtual method for getting TimeCode value stored in this SpaceWire IF.
	 * @return 2bit Flag + 6bit TimeCode
	 */
	virtual unsigned char getTimeCode() throw(SpaceWireException) =0;

public:
	/** A method sets an action against getting TimeCode.
	 */
	void setTimeCodeAction(SpaceWireTimeCodeAction* action);

public:
	/** Getter for the SpaceWire IF port number.
	 * @return instance's port number
	 */
	int getPortNumber();

	/** Setter for Debug mode.
	 * Definition of debug mode varies with implementations.
	 * One implementation may dump every packet at send() in
	 * receive() methods.
	 */
	inline void setDebugMode(){
		debugmode=true;
	}

	/** Setter for Debug mode.
	 */
	inline void unsetDebugMode(){
		debugmode=false;
	}

	/** Setter for Debug mode.
	 */
	inline void toggleDebugMode(){
		if(debugmode==true){
			debugmode=false;
		}else{
			debugmode=true;
		}
	}

	/** Getter for Debug mode.
	 * Programmer, who implements new child class of SpaceWireIF,
	 * can use this method to check if the instance is set to
	 * debug mode and perform special actions like dump packet.
	 * @return true if in Debug mode
	 */
	inline bool isDebugMode(){
		return debugmode;
	}
};
#endif /*SPACEWIREIF_HH_*/
