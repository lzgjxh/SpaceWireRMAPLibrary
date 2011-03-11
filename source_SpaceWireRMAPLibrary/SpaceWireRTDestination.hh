#ifndef SPACEWIRERTDESTINATION_HH_
#define SPACEWIRERTDESTINATION_HH_

#include "SpaceWireDestination.hh"

#include <vector>
using namespace std;

class SpaceWireRTDestination : public SpaceWireDestination {
private:
	/** Source Logical Address. 
	 */
	unsigned char sourceLogicalAddress;
	
	/** Source Path Address.
	 * A path address which is needed to transport an
	 * RMAP packet in Path Addressing mode.
	 */
	vector<unsigned char> sourcePathAddress;
	
	/** Channel number.
	 */
	unsigned char channel;
	
public:
	SpaceWireRTDestination();
public:
	static const unsigned char DefaultChannelNumber=0x00;
	static const unsigned char MinimumChannelNumber=0x00;
	static const unsigned char MaximumChannelNumber=0xFF;
	
};


#endif /*SPACEWIRERTDESTINATION_HH_*/

/** History
 * 2008-08-26 file created (Takayuki Yuasa)
 */