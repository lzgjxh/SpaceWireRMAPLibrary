#ifndef SPACEWIREIFCOMMANDLINEIF_HH_
#define SPACEWIREIFCOMMANDLINEIF_HH_

#include "SpaceWireAsynchronousStatus.hh"
#include "SpaceWireException.hh"
#include "SpaceWireIF.hh"
#include "SpaceWireUtilities.hh"

#include <fstream>
#include <iostream>
#include <iomanip>
using namespace std;

/** Dummy SpaceWire IF class. This class may be used in a test case.
 * By using this class, users can observe a raw packet which is sent
 * from an upper layer application such as an RMAPSocket, etc.
 * This IF class also provides a method for users to fabricate a reply
 * packet, which is sent back to the upper layer applications,
 * via command line input.
 */
class SpaceWireIFCommandLineIF : public SpaceWireIF {
private:
	bool opened;
	static const unsigned int BufferSize=2048; //2kbytes buffer
	unsigned short* buffer;
private:
	unsigned char timecode;
public:
	SpaceWireIFCommandLineIF();
	~SpaceWireIFCommandLineIF();
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
private:
	int selectSendMenu();
	void savePacketToFile(vector<unsigned char> packet);

	int selectReceiveMenu();
	vector<unsigned char> commandLinePacketCreation();
	vector<unsigned char> fileStreamPacketCreation();
};

#endif /*SPACEWIREIFCOMMANDLINEIF_HH_*/
