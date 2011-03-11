#ifndef SPACEWIREIFDOMESTICPIPE_HH_
#define SPACEWIREIFDOMESTICPIPE_HH_

#include "SpaceWireAsynchronousStatus.hh"
#include "SpaceWireException.hh"
#include "SpaceWireIF.hh"
class SpaceWireIFDomesticPipe;
#include "SpaceWireIFDomesticPipeCore.hh"
#include "SpaceWireUtilities.hh"

#include <fstream>
#include <iostream>
#include <iomanip>
using namespace std;

/** Dummy SpaceWire IF class. This class may be used in a test case.
 * This SpaceWire IF class provides user programs a virtual Tx and Rx
 * buffers. Those buffers can be accessed via send() and receive()
 * methods from multiple threads.
 *
 * As shown in the diagram below, two programs or two threads in a
 * program can communicate via SpaceWireIF using this pipe I/F class
 * and SpaceWireIFDomesticPipeCore class.
 *
 *   [User Progarm A (class)]
 *            || send/receive
 *  [SpaceWireIFDomesticPipe]
 *            ||
 * [SpaceWireIFDomesticPipeCore]
 *            ||
 *  [SpaceWireIFDomesticPipe]
 *            || send/receve
 *   [User Progarm B (class)]
 *
 */
class SpaceWireIFDomesticPipe : public SpaceWireIF {
private:
	SpaceWireIFDomesticPipeCore* domesticpipecore;
	unsigned int pipeendtype;
public:
	SpaceWireIFDomesticPipe(SpaceWireIFDomesticPipeCore* newcore, unsigned int newpipeendtype);
	~SpaceWireIFDomesticPipe();
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

#endif /*SPACEWIREIFDOMESTICPIPE_HH_*/
