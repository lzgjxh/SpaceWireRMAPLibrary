#ifndef SPACEWIREIFCONNECTEDTODUMMYRMAPTARGET_HH_
#define SPACEWIREIFCONNECTEDTODUMMYRMAPTARGET_HH_

#include "RMAPPacket.hh"
#include "SpaceWireAsynchronousStatus.hh"
#include "SpaceWireException.hh"
#include "SpaceWireIF.hh"
#include "SpaceWireUtilities.hh"

#include "Mutex.hh"
#include "Condition.hh"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
using namespace std;

/** SpaceWire IF class which imitates a general RMAP Packet.
 * If send method of this class is invoked with an RMAP
 * Command packet, this class hooks and interpret the command,
 * then cteates and send-back an RMAP Reply.
 * This class can be used as a SpaceWireIF simulator on PC.
 */
class SpaceWireIFConnectedToDummyRMAPTarget : public SpaceWireIF {
private:
	bool opened;
	unsigned short* buffer;
	queue<RMAPPacket*> packetqueue;
	Condition condition;
	Mutex packetqueuemutex;
private:
	unsigned char timecode;
public:
	SpaceWireIFConnectedToDummyRMAPTarget();
	~SpaceWireIFConnectedToDummyRMAPTarget();
	bool initialize() throw(SpaceWireException);
	bool open() throw(SpaceWireException);
	bool open(int portnumber) throw(SpaceWireException);
	void close();
	void startLinkInitializationSequenceAsAMasterNode() throw(SpaceWireException);
	void startLinkInitializationSequenceAsASlaveNode() throw(SpaceWireException);
	void stopLink() throw(SpaceWireException);
	void resetStateMachine() throw(SpaceWireException);
	void clearSendFIFO() throw(SpaceWireException);
	void clearReceiveFIFO() throw(SpaceWireException);
	bool reset() throw(SpaceWireException);

	void finalize() throw(SpaceWireException);
	void send(vector<unsigned char>* packet) throw(SpaceWireException);
	vector<unsigned char> receive() throw(SpaceWireException);
	void receive(vector<unsigned char>* packet) throw(SpaceWireException);
	void abort() throw(SpaceWireException);
	SpaceWireAsynchronousStatus asynchronousReceive() throw(SpaceWireException);
	SpaceWireAsynchronousStatus asynchronousSend(vector<unsigned char>* packet)	throw(SpaceWireException);
	void waitCompletionOfAsynchronousOperation(SpaceWireAsynchronousStatus status)
		throw(SpaceWireException);

	void sendTimeCode(unsigned char flag_and_timecode) throw(SpaceWireException);
	unsigned char getTimeCode() throw(SpaceWireException);
};


#endif /*SPACEWIREIFCONNECTEDTODUMMYRMAPTARGET_HH_*/
