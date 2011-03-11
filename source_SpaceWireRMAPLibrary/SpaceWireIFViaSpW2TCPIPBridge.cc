#include "SpaceWireIFViaSpW2TCPIPBridge.hh"
#include "Condition.hh"
using namespace std;

//#define DEBUG

SpaceWireIFViaSpW2TCPIPBridge::SpaceWireIFViaSpW2TCPIPBridge(string newhostname) : SpaceWireIFOverIPClient(newhostname,spw2tcpiphongo_dataport) {}

SpaceWireIFViaSpW2TCPIPBridge::~SpaceWireIFViaSpW2TCPIPBridge(){}

/** History
 * 2008-08-26 The implementation of SSDTPModule was moved to SpaceWireIFOverIPClient.cc
 */
