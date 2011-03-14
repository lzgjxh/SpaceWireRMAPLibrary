#ifndef SPACEWIREIFSPACECUBE1SHIMAFUJIROUTERIPCORE_HH_
#define SPACEWIREIFSPACECUBE1SHIMAFUJIROUTERIPCORE_HH_

#include "SpaceWireIF.hh"
#include "SpaceWireUtilities.hh"

extern "C" {
	#include "spw_if_router.h"
}

#include <stdlib.h>
#include <btron/util.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
using namespace std;


/** SpaceWire IF wrapper class for Shimafuji SpaceWire IP core with Router
 * on SpaceCube 1.
 */
class SpaceWireIFSpaceCube1ShimafujiRouterIPcore : public SpaceWireIF {
private:
	bool opened;
	static const unsigned int BufferSize=51200;
	int devicedescriptor;
	UW* sendbuffer;
	vector<unsigned char>* receivebuffer;
public:
	SpaceWireIFSpaceCube1ShimafujiRouterIPcore();
	SpaceWireIFSpaceCube1ShimafujiRouterIPcore(unsigned int newport);
	~SpaceWireIFSpaceCube1ShimafujiRouterIPcore();
	bool initialize() throw(SpaceWireException);
	bool open() throw(SpaceWireException);
	bool open(int portnum) throw(SpaceWireException);
	void close();
	void startLinkInitializationSequenceAsAMasterNode() throw(SpaceWireException);
	void startLinkInitializationSequenceAsASlaveNode() throw(SpaceWireException);
	void stopLink() throw(SpaceWireException);
	SpaceWireLinkStatus getLinkStatus() throw(SpaceWireException);
	void setLinkStatus(SpaceWireLinkStatus newstatus) throw(SpaceWireException);
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
	static const unsigned int SHIMAFUJI_ROUTERIPCORE_DEFAULT_PORTNUMBER=4;
	static const unsigned int SHIMAFUJI_ROUTERIPCORE_PORTNUMBER_MINIMUM=4;
	static const unsigned int SHIMAFUJI_ROUTERIPCORE_PORTNUMBER_MAXIMUM=6;
};

#endif /*SPACEWIREIFSPACECUBE1SHIMAFUJIROUTERIPCORE_HH_*/


/** History
 * 2008-09-16 file created (Takayuki Yuasa)
 * 2008-12-17 added TimeCode related methods (Takayuki Yuasa)
 */
