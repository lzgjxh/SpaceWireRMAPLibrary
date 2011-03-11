#ifndef RMAPSOCKET_HH_
#define RMAPSOCKET_HH_

#include "RMAPEngine.hh"
#include "RMAPPacket.hh"

#include "Mutex.hh"

#include <iostream>
using namespace std;

class RMAPEngine; //dummy


/** This class provides simple RMAP Read/Write methods
 * to users. RMAPSocket creates/interprets RMAP Packet
 * automatically, and communicates with RMAPEngine to
 * transfer created packets. Each RMAP Target in a
 * SpaceWire network is expressed as an instance of
 * RMAPSocket in user program. Multiple transaction can
 * be performed with multiple RMAPSockets.
 */ 
class RMAPSocket {
private:
	/** A reference to RMAPEngine class with which this RMAPSocket
	 * communicates.
	 */
	RMAPEngine* parentrmapengine;
	
	/** ID of this RMAPSocket. This is allocated by RMAPEngine when
	 * the socket is created, and it is used to distinguish multiple
	 * sockets in RMAPEngine.
	 */
	unsigned int socketid;
	
	/** RMAPPacket instance for RMAP Command Packet.
	 */
	RMAPPacket* commandpacket;

	/** RMAPPacket instance for RMAP Reply Packet.
	 */
	RMAPPacket* replypacket;
	
	/** RMAPDestination to which this RMAPSocket is connected.
	 */
	RMAPDestination rmapdestination;
	
	/** Time out mode.
	 */
	bool timeoutenabled;
	
	/** Automatic Retry in case of Time out.
	 */
	bool automaticretryenabled;

	/** Duration of time out.
	 */
	unsigned int timeoutduration;

	Mutex readmutex;
	Mutex writemutex;
public:
	/** Constructor.
	 */
	RMAPSocket();
	
	/** Constructor.
	 * @param newsocketid socket id
	 */
	RMAPSocket(unsigned int newsocketid);
	
	/** Constructor.
	 * @param newsocketid socket id
	 * @param newrmapengine a reference to parent RMAPEngine
	 */
	RMAPSocket(unsigned int newsocketid,RMAPEngine* newrmapengine);

	/** Constructor.
	 * @param newsocketid socket id
	 * @param newrmapengine a reference to parent RMAPEngine
	 * @param newrmapdestination an RMAPDestination to which this instance is connected
	 */
	RMAPSocket(unsigned int newsocketid,RMAPEngine* newrmapengine,RMAPDestination newrmapdestination);

	/** A private method which initializes the private membbers.
	 */
	void initialize();
public:
	/** RMAP Read. This method waits until the read access is
	 * successfully performed or an exception is thrown.
	 * @param address register or memory address to be read
	 * @param length length of data to be read
	 * @return a reference to read data vector
	 */
	vector<unsigned char>* read(unsigned int address,unsigned int length) throw(RMAPException);
	
	/** RMAP Write. This method waits until the write access is
	 * successfully performed or an exception is thrown.
	 * @param address register or memory address where data are written
	 * @param data a reference to data vector to be written
	 */
	void write(unsigned int address, vector<unsigned char>* data) throw(RMAPException);
	
	/** Read-Modify-Write. This method might not be implemented (mendou).
	 * Dareka, implement shitara, renraku shite kudasai.
	 */
	void readModifyWrite(unsigned int address,vector<unsigned char>* data, unsigned int length) throw(RMAPException);
	
public:
	/** Returns the latest Reply Packet.
	 * @return Reply Packet
	 */
	RMAPPacket getTheLastReplyPacket();

	/** Returns the latest Command Packet.
	 * @return Command Packet
	 */
	RMAPPacket getTheLastCommandPacket();
public:
	/** Getter for socket id.
	 * @return socket id
	 */
	unsigned int getSocketID();
	
	/** Setter for socket id.
	 * @param newsocketid socket id
	 */
	void setSocketID(unsigned int newsocketid);
	
	/** Getter for the parent RMAPEngine.
	 * @return a reference to the parent RMAPEngine instance
	 */
	RMAPEngine* getParentRMAPEngine();
	
	/** Setter for the parent RMAPEngine.
	 * @param newrmapengine a reference to the parent RMAPEngine instance
	 */
	void setParentRMAPEngine(RMAPEngine* newrmapengine);
	
	/** Setter for time out mode.
	 * @param newtimeoutduration time out duration in milli second
	 */
	void enableTimeout(unsigned int newtimeoutduration);
	
	/** Setter for time out mode.
	 */
	void enableTimeout();
	
	/** Setter for time out mode.
	 */
	void disableTimeout();
	
	/** Getter for time out mode.
	 * @return true if time out mode is enabled
	 */
	bool isTimeoutEnabled();
	
	/** Setter for automatic reply mode.
	 */
	void enableAutomaticRetry();

	/** Setter for automatic reply mode.
	 */
	void disableAutomaticRetry();

	/** Getter for automatic retry mode.
	 * @return true if automatic retry is enabled
	 */
	bool isAutomaticRetryEnabled();

	/** Getter for time out mode.
	 * @return time out duration
	 */
	unsigned int getTimeoutDuration() throw (RMAPException);
	
	/** Setter for CRC Check mode.
	 */
	void enableCRCCheck();
	
	/** Setter for CRC Check mode.
	 */
	void disableCRCCheck();
	
	/** Getter for CRC Check mode.
	 */
	inline bool isCRCCheckEnabled(){
		return crccheck;
	}
	
	/** Increments Request counter.
	 */
	inline void incrementCounter(){
		requestcounter++;
	}
	
	/** Resets Request counter.
	 */
	inline void resetCounter(){
		requestcounter=0;
	}
	
	/** Returns Request counter.
	 */
	inline int getCounter(){
		return requestcounter;
	}
	
	/** Returns RMAPDestination to which this
	 * RMAPSocket is connected.
	 * @return an instance of RMAPDestination
	 */
	inline RMAPDestination getRMAPDestination(){
		return rmapdestination;
	}
	
	/** Return packet pointer.
	 */
	inline RMAPPacket* getCommandPacketPointer(){
		return commandpacket;
	}

	/** Setter for RMAPDestination pointer.
	 * @param newrmapdestination an RMAPDestination instance
	 */
	inline void setRMAPDestination(RMAPDestination newrmapdestination){
		rmapdestination=newrmapdestination;
	}
private:
	int requestcounter;
	
private:
	inline void applyCRCCheckMode(){
		if(crccheck){
			commandpacket->enableCRCCheck();
			replypacket->enableCRCCheck();
		}else{
			commandpacket->disableCRCCheck();
			replypacket->disableCRCCheck();
		}
	}
	bool crccheck;
	
public:
	static const bool TimeoutModeEnabled=true;
	static const bool TimeoutModeDisabled=false;
	static const bool AutomaticRetryEnabled=true;
	static const bool AutomaticRetryDisabled=false;
	
	static const unsigned int DefaultSocketID=0;
	static const unsigned int DefaultTimeoutDurationInMilliSecond=1000;
	static const bool DefaultTimeoutMode=TimeoutModeDisabled;
	static const bool DefaultAutomaticRetryMode=AutomaticRetryEnabled;
};

#endif /*RMAPSOCKET_HH_*/

/** History
 * 2008-05-xx file created (Takayuki Yuasa)
 * 2008-10-03 CRC check mode accessor aaded (Takayuki Yuasa)
 * 2008-10-04 a counter which counts Request number added (Takayuki Yuasa)
 * 2010-01-27 Automatic Reply mode was added (Takayuki Yuasa)
 */
