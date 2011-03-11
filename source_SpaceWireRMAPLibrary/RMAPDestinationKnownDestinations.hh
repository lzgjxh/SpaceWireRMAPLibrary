#ifndef RMAPDESTINATIONKNOWNDESTINATIONS_HH_
#define RMAPDESTINATIONKNOWNDESTINATIONS_HH_

#include "RMAPDestination.hh"

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
using namespace std;

class RMAPDestinationKnownDestinations {
private:
	RMAPDestinationKnownDestinations(){}

public:
	static RMAPDestination* selectInstanceFromCommandLineMenu();
private:
	static void inputNewRMAPDestination(RMAPDestination* rmapdestination);
	static void setDefaultRMAPDestination(RMAPDestination* rmapdestination);
	static void dumpCurrentRMAPDestination(RMAPDestination* rmapdestination);
	static void setForSpaceWireIFBoardsByShimafujiDraftELogicalAddress0x02(RMAPDestination* rmapdestination);
	static void setForSpaceWireIFBoardsByShimafujiDraftELogicalAddress0xFE(RMAPDestination* rmapdestination);
	static void setForSpaceWireIFBoardsByShimafujiDraftFLogicalAddress0xFE(RMAPDestination* rmapdestination);
	static void setForSpaceWireIFBoardsByShimafujiDraftELogicalAddress0x30(RMAPDestination* rmapdestination);
	static void setForSpaceWireIFBoardsByShimafujiDraftFLogicalAddress0x30(RMAPDestination* rmapdestination);
	static void setForSpaceCubeRouterConfigurationRegister(RMAPDestination* rmapdestination);
	static void setForSpaceWireRouterByNEC(RMAPDestination* rmapdestination);
	static void setForSpaceWireUniversalIOBoardByMHI(RMAPDestination* rmapdestination);
};

#endif /*RMAPDESTINATIONKNOWNDESTINATIONS_HH_*/

/** History
 * 2008-10-07 file created (Takayuki Yuasa)
 */
