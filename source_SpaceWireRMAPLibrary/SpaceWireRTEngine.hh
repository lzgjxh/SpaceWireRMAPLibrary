#ifndef SPACEWIRERTENGINE_HH_
#define SPACEWIRERTENGINE_HH_

#include "SpaceWireRTChannel.hh"

#include "Condition.hh"

#include <map>
#include <vector>
#include <iostream>
using namespace std;

class SpaceWireRTEngine {
	/** RTのチャネルやタイムスロットなどの資源管理を行う
	 * 個々のセッションの管理はSpaceWireRTSessionクラスに
	 * 担当させる？
	 */
private:
	map<unsigned char,SpaceWireRTChannel*> channelmap;
	map<unsigned char,SpaceWireDestination*> destinationmap;
public:
	SpaceWireRTChannel* createChannel(SpaceWireRTDestination* newspacewirertdestination);
	void deleteChannel(SpaceWireRTChannel* newspacewirertchannel);
};

#endif /*SPACEWIRERTENGINE_HH_*/

/** History
 * 2008-08-26 file created (Takayuki Yuasa)
 */