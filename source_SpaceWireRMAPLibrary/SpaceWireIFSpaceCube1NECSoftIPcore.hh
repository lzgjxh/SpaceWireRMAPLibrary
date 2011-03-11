#ifndef SPACEWIREIFSPACECUBE1NECSOFTIPCORE_HH_
#define SPACEWIREIFSPACECUBE1NECSOFTIPCORE_HH_

#ifdef USE_SPC1_NEC_IP_CORE

#include "SpaceWireIF.hh"
#include "SpaceWireUtilities.hh"

#include "libspw.h"

#include <btron/util.h>
#include <iomanip>
using namespace std;

/** SpaceWire IF wrapper class for NECSoft SpaceWire IP core on SpaceCube 1.
 * The api functions of the SpaceWire IF driver/library are wrapped to methods
 * listed in SpaceWireIF.hh.
 */
class SpaceWireIFSpaceCube1NECSoftIPcore : public SpaceWireIF {
private:
	/** a struct for SpaceWire device defined in
	 * the header file of NES SpW Library (libspw.h).
	 */
	HANDLE spwhandler;

	bool opened;
	static const unsigned int SendBufferSize=4096; //4kbytes buffer
	static const unsigned int ReceiveBufferSize=4096; //4kbytes buffer
	PVOID sendbuffer;
	PVOID receivebuffer;
public:
	/** Constructor.
	 */
	SpaceWireIFSpaceCube1NECSoftIPcore();
	/** Constructor.
	 * @param newportnumber a port number to be opened
	 */
	SpaceWireIFSpaceCube1NECSoftIPcore(unsigned int newportnumber);
	~SpaceWireIFSpaceCube1NECSoftIPcore();
	bool initialize() throw(SpaceWireException);
	bool open() throw(SpaceWireException);
	bool open(int portnumber) throw(SpaceWireException);
	void close();
	void startLinkInitializationSequenceAsAMasterNode() throw(SpaceWireException);
	void startLinkInitializationSequenceAsASlaveNode() throw(SpaceWireException);
	void stopLink() throw(SpaceWireException);
	SpaceWireLinkStatus setLinkStatus() throw(SpaceWireException) throw(SpaceWireException);
	void setLinkStatus(SpaceWireLinkStatus newstatus) throw(SpaceWireException);
	void resetStateMachine() throw(SpaceWireException);
	void clearSendFIFO() throw(SpaceWireException);
	void clearReceiveFIFO() throw(SpaceWireException);
	bool reset() throw(SpaceWireException);

	void finalize() throw(SpaceWireException);
	void send(vector<unsigned char>* packet) throw(SpaceWireException);
	vector<unsigned char> sendreceive(vector<unsigned char>* packet) throw(SpaceWireException);
	vector<unsigned char> receive() throw(SpaceWireException);
	void receive(vector<unsigned char>* packet) throw(SpaceWireException);
	void abort() throw(SpaceWireException);
	SpaceWireAsynchronousStatus asynchronousReceive() throw(SpaceWireException);
	SpaceWireAsynchronousStatus asynchronousSend(vector<unsigned char>* packet)	throw(SpaceWireException);
	void waitCompletionOfAsynchronousOperation(SpaceWireAsynchronousStatus status) throw(SpaceWireException);

	void sendTimeCode(unsigned char flag_and_timecode) throw(SpaceWireException);
	unsigned char getTimeCode() throw(SpaceWireException);
private:
	void allocateBuffer();
public:
	static const int DEFAULT_PORTNUMBER=0;
	static const int PORTNUMBER_MINIMUM=0;
	static const int PORTNUMBER_MAXIMUM=2;
};

#endif

#endif /*SPACEWIREIFSPACECUBE1NECSOFTIPCORE_HH_*/
