#ifndef SPACEWIREIFVIASPW2TCPIPHONGO_HH_
#define SPACEWIREIFVIASPW2TCPIPHONGO_HH_

#include "IPClientSocket.hh"

#include "SpaceWireIFOverIPClient.hh"
#include "SpaceWireUtilities.hh"

using namespace std;


/** SpaceWire IF class which is connected to a real SpaceWire IF
 * via TCP/IP network and spw-tcpip bridge server running on SpaceCube.
 */
class SpaceWireIFViaSpW2TCPIPBridge : public SpaceWireIFOverIPClient {
public:
	SpaceWireIFViaSpW2TCPIPBridge(string newhostname);
	~SpaceWireIFViaSpW2TCPIPBridge();
public:
	static const unsigned short spw2tcpiphongo_dataport=10030;
};

#endif /*SPACEWIREIFVIASPW2TCPIPHONGO_HH_*/

/** History
 * 2008-08-26 The declaration of SSDTPModule was moved to SpaceWireIFOverIPClient.hh
 */
