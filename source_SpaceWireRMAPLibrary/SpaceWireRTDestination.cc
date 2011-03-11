#include "SpaceWireRTDestination.hh"
using namespace std;

SpaceWireRTDestination::SpaceWireRTDestination(){
	channel=SpaceWireRTDestination::DefaultChannelNumber;
	sourcePathAddress.clear();
}

/** History
 * 2008-08-26 file created (Takayuki Yuasa)
 */