#ifndef SPACEWIRERTCHANNEL_HH_
#define SPACEWIRERTCHANNEL_HH_

#include "SpaceWireRTDestination.hh"

#include <vector>
using namespace std;

class SpaceWireRTChannel : public SpaceWireIF {
	/** SpaceWireIFを継承しており、上位レイヤー
	 * (RMAPEngineなど)が利用する、仮想的な
	 * SpaceWireIFにもなる
	 */

	/** 行き先、チャネル番号などを保持
	 * ユーザからみると、このオブジェクトが
	 * Channelに対応しており、バッファへの投げ込み、
	 * バッファからの受け取りが可能
	 */
private:
	SpaceWireRTDestination spacewirertdestination;
	list< vector<unsigned char> >* datalist;
	
public:
	
};

#endif /*SPACEWIRERTCHANNEL_HH_*/
