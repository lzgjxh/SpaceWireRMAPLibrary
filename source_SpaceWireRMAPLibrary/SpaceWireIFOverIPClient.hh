#ifndef SPACEWIREIFOVERIPCLIENT_HH_
#define SPACEWIREIFOVERIPCLIENT_HH_

#include "IPClientSocket.hh"

#include "SpaceWireIF.hh"
#include "SpaceWireUtilities.hh"
#include "SpaceWireSSDTPModule.hh"

#include <exception>
#include <iostream>
#include <string>
#include <vector>
using namespace std;


/** SpaceWire IF class which is connected to a real SpaceWire IF
 * via TCP/IP network and spw-tcpip bridge server running on SpaceCube.
 */
class SpaceWireIFOverIPClient : public SpaceWireIF, public SpaceWireTimeCodeAction {
private:
	bool opened;
private:
	string iphostname;
	unsigned int ipportnumber;
	SSDTPModule* ssdtp;
	IPClientSocket* datasocket;
public:
	SpaceWireIFOverIPClient(string newiphostname, unsigned int newipportnumber);
	~SpaceWireIFOverIPClient();
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
	void receive(vector<unsigned char>* packet) throw(SpaceWireException);
	vector<unsigned char> receive() throw(SpaceWireException);
	void abort() throw(SpaceWireException);
	SpaceWireAsynchronousStatus asynchronousReceive() throw(SpaceWireException);
	SpaceWireAsynchronousStatus asynchronousSend(vector<unsigned char>* packet)	throw(SpaceWireException);
	void waitCompletionOfAsynchronousOperation(SpaceWireAsynchronousStatus status)
		throw(SpaceWireException);

	void sendTimeCode(unsigned char flag_and_timecode) throw(SpaceWireException);
	unsigned char getTimeCode() throw(SpaceWireException);
	virtual void doAction(unsigned char timecode);
public:
	void setTxDivCount(unsigned char txdivcount);
};

#endif /*SPACEWIREIFOVERIPCLIENT_HH_*/

/** History
 * 2008-08-26 file created (Takayuki Yuasa)
 */
