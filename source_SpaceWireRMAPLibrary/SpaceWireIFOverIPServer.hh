#ifndef SPACEWIREIFOVERIPSERVER_HH_
#define SPACEWIREIFOVERIPSERVER_HH_

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
class SpaceWireIFOverIPServer : public SpaceWireIF {
private:
	bool opened;
private:
	string iphostname;
	unsigned int ipportnumber;
	SSDTPModule* ssdtp;
	IPServerSocket* listeningsocket;
	IPServerAcceptedSocket* acceptedsocket;
	Mutex openclosemutex;
public:
	SpaceWireIFOverIPServer(unsigned int newipportnumber);
	~SpaceWireIFOverIPServer();
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
public:
	static const unsigned short spw2tcpiphongo_dataport=10030;
};

#endif /*SPACEWIREIFOVERIPSERVER_HH_*/

/** History
 * 2008-08-26 file created (Takayuki Yuasa)
 */
