#ifndef SPACEWIREIFIMPLEMENTATIONS_HH_
#define SPACEWIREIFIMPLEMENTATIONS_HH_

//system dependent implementations
#ifdef SYSTEM_TYPE_POSIX
#include "SpaceWireIFViaSpW2TCPIPBridge.hh"
#endif

#ifdef SYSTEM_TYPE_T_KERNEL
#ifdef USE_SPC1_NEC_IP_CORE
#include "SpaceWireIFSpaceCube1NECSoftIPcore.hh"
#endif
#include "SpaceWireIFSpaceCube1ShimafujiIPcore.hh"
#include "SpaceWireIFSpaceCube1ShimafujiRouterIPcore.hh"
#endif

//portable implementations
#include "SpaceWireIFCommandLineIF.hh"
#include "SpaceWireIFDomesticPipe.hh"
#include "SpaceWireIFOverIPClient.hh"
#include "SpaceWireIFOverIPServer.hh"

#include "SpaceWireCLI.hh"

#include <iostream>
using namespace std;

class SpaceWireIFImplementations {
private:
	SpaceWireIFImplementations(){}
	//void virtualfunction() = 0;
public:
	static SpaceWireIF* selectInstanceFromCommandLineMenu() throw(SpaceWireException);
	static SpaceWireIF* createInstanceFromName(string spacewireifname) throw(SpaceWireException);
};

#endif /*SPACEWIREIFIMPLEMENTATIONS_HH_*/

/** History
 * 2008-09-02 file created (Takayuki Yuasa)
 * 2008-09-04 ifdef senetences are added for NEC Soft IP core (Takayuki Yuasa)
 * 2008-12-05 createInstanceFromName() added (Takayuki Yuasa)
 */
