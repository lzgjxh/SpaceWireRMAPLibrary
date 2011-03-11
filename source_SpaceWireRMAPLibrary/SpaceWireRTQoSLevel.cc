#include "SpaceWireRTQoSLevel.hh"
using namespace std;

SpaceWireRTQoSLevel::SpaceWireRTQoSLevel(){
	leveltype=DefaultQoSLevel;
}

SpaceWireRTQoSLevel::SpaceWireRTQoSLevel(int newleveltype)  throw(SpaceWireRTException){
	setQoSLevel(newleveltype);
}

SpaceWireRTQoSLevel::~SpaceWireRTQoSLevel(){}

int SpaceWireRTQoSLevel::getQoSLevel(){
	return leveltype;
}

void SpaceWireRTQoSLevel::setQoSLevel(int newleveltype)  throw(SpaceWireRTException){
	if(isValidLevelType(newleveltype)){
		leveltype=newleveltype;
	}else{
		throw SpaceWireRTException(SpaceWireRTException::InvalidQoSLevel);
	}
}

bool SpaceWireRTQoSLevel::isValidLevelType(int newleveltype){
	if(newleveltype==BestEffortQoS ||
			newleveltype==AssuredQoS ||
			newleveltype==ReservedQoS ||
			newleveltype==GuaranteedQoS ){
		return true;
	}else{
		return false;
	}
}

/** History
 * 2008-08-26 file created (Takayuki Yuasa)
 */