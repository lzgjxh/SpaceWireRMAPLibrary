#ifndef SPACEWIREIFSPACECUBE1SHIMAFUJIIPCORE_HH_
#define SPACEWIREIFSPACECUBE1SHIMAFUJIIPCORE_HH_

#include "SpaceWireIF.hh"
#include "SpaceWireUtilities.hh"

extern "C" {
	#include "spw_if.h"
}

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <btron/util.h>
using namespace std;


/** SpaceWire IF wrapper class for Shimafuji SpaceWire IP core on SpaceCube 1.
 * The api functions of the SpaceWire IF driver/library are wrapped to methods
 * listed in SpaceWireIF.hh.
 */
class SpaceWireIFSpaceCube1ShimafujiIPcore : public SpaceWireIF {
private:
	bool opened;
	static const unsigned int BufferSize=8192;
	UW* sendbuffer;
	vector<unsigned char>* receivebuffer;
	int devicedescriptor;
public:
	SpaceWireIFSpaceCube1ShimafujiIPcore();
	SpaceWireIFSpaceCube1ShimafujiIPcore(int newportnumber);
	~SpaceWireIFSpaceCube1ShimafujiIPcore();
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
	void receive(unsigned char* array,unsigned int* receivedsize);
	void abort() throw(SpaceWireException);
	vector<unsigned char> receive() throw(SpaceWireException);
	SpaceWireAsynchronousStatus asynchronousReceive() throw(SpaceWireException);
	SpaceWireAsynchronousStatus asynchronousSend(vector<unsigned char>* packet)	throw(SpaceWireException);
	void waitCompletionOfAsynchronousOperation(SpaceWireAsynchronousStatus status) throw(SpaceWireException);

	void sendTimeCode(unsigned char flag_and_timecode) throw(SpaceWireException);
	unsigned char getTimeCode() throw(SpaceWireException);
public:
	static const unsigned int SHIMAFUJI_IPCORE_DEFAULT_PORTNUMBER=1;
};

#endif /*SPACEWIREIFSPACECUBE1SHIMAFUJIIPCORE_HH_*/

/** History
 * 2008-05-xx file created (Takayuki Yuasa)
 * 2008-09-02 packet dump in debug mode added (Takahisa Fujinaga)
 */
