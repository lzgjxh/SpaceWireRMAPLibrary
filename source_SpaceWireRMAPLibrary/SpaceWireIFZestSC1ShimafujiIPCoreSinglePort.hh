#ifndef SPACEWIREIFZESTSC1SHIMAFUJIIPCORESINGLEPORT_HH_
#define SPACEWIREIFZESTSC1SHIMAFUJIIPCORESINGLEPORT_HH_

#include "SpaceWireIF.hh"
#include "SpaceWireUtilities.hh"

#include <iostream>
#include <iomanip>
#include <stdlib.h>
using namespace std;


/** SpaceWire IF wrapper class for Shimafuji SpaceWire IP core on ZestSC1.
 * The api functions of the SpaceWire IF driver/library are wrapped to methods
 * listed in SpaceWireIF.hh.
 */
class SpaceWireIFZestSC1ShimafujiIPCoreSinglePort : public SpaceWireIF {
private:
	bool opened;
	static const unsigned int BufferSize=8192;
public:
	SpaceWireIFZestSC1ShimafujiIPCoreSinglePort();
	SpaceWireIFZestSC1ShimafujiIPCoreSinglePort(int newportnumber);
	~SpaceWireIFZestSC1ShimafujiIPCoreSinglePort();
	bool initialize() throw(SpaceWireException);
	bool open() throw(SpaceWireException);
	bool open(int newportnumber) throw(SpaceWireException);
	void close();
	void startLinkInitializationSequenceAsAMasterNode() throw(SpaceWireException);
	void startLinkInitializationSequenceAsASlaveNode() throw(SpaceWireException);
	void stopLink() throw(SpaceWireException);
	SpaceWireLinkStatus getLinkStatus() throw(SpaceWireException);
	void setLinkStatus(SpaceWireLinkStatus newlinkstatus) throw(SpaceWireException);
	void resetStateMachine() throw(SpaceWireException);
	void clearSendFIFO() throw(SpaceWireException);
	void clearReceiveFIFO() throw(SpaceWireException);
	bool reset() throw(SpaceWireException);

	void finalize() throw(SpaceWireException);
	void send(vector<unsigned char>* packet) throw(SpaceWireException);
	void receive(vector<unsigned char>* packet) throw(SpaceWireException);
	void abort() throw(SpaceWireException);
	vector<unsigned char> receive() throw(SpaceWireException);
	SpaceWireAsynchronousStatus asynchronousReceive() throw(SpaceWireException);
	SpaceWireAsynchronousStatus asynchronousSend(vector<unsigned char>* packet)	throw(SpaceWireException);
	void waitCompletionOfAsynchronousOperation(SpaceWireAsynchronousStatus status) throw(SpaceWireException);

	void sendTimeCode(unsigned char flag_and_timecode) throw(SpaceWireException);
	unsigned char getTimeCode() throw(SpaceWireException);
public:
	static const unsigned int SHIMAFUJI_IPCORE_DEFAULT_PORTNUMBER=1;
	static const unsigned int AddressOfReceiveBufferReadyRegister=0x2100;

};

#endif /*SPACEWIREIFZESTSC1SHIMAFUJIIPCORESINGLEPORT_HH_*/

/** History
 * 2009-07-17 file created (Takayuki Yuasa)
 */
