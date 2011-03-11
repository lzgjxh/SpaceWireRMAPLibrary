#ifndef SPACEWIRE_HH_
#define SPACEWIRE_HH_

/** Header fiels needed to be included in a SpaceWire related programs.
 */

#include "SpaceWireAsynchronousStatus.hh"
#include "SpaceWireCLI.hh"
#include "SpaceWireDestination.hh"
#include "SpaceWireException.hh"
#include "SpaceWireExceptionHandler.hh"
#include "SpaceWireIF.hh"
#include "SpaceWireIFCommandLineIF.hh"
#include "SpaceWireIFConnectedToDummyRMAPTarget.hh"
#include "SpaceWireIFDomesticPipe.hh"
#include "SpaceWireIFDomesticPipeCore.hh"
#include "SpaceWireIFImplementations.hh"
#include "SpaceWireIFOverIPClient.hh"
#include "SpaceWireIFOverIPServer.hh"

#ifdef SYSTEM_TYPE_POSIX

#include "Types.hh"

#endif

#ifdef SYSTEM_TYPE_T_KERNEL
#include "SpaceWireIFSpaceCube1ShimafujiIPcore.hh"
#include "SpaceWireIFSpaceCube1ShimafujiRouterIPcore.hh"

#ifdef USE_SPC1_NEC_IP_CORE
#include "SpaceWireIFSpaceCube1NECSoftIPcore.hh"
#endif

#endif

#include "SpaceWireIFViaSpW2TCPIPBridge.hh"
#include "SpaceWireLinkStatus.hh"
#include "SpaceWirePacket.hh"

#endif /*SPACEWIRE_HH_*/

/** History
 * 2008-08-26 file created (Takayuki Yuasa)
 */
