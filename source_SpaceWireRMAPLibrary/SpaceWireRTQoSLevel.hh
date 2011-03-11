#ifndef SPACEWIRERTQOSLEVEL_HH_
#define SPACEWIRERTQOSLEVEL_HH_

#include "SpaceWireRTException.hh"

using namespace std;

class SpaceWireRTQoSLevel {
private:
	int leveltype;
public:
	static const int BestEffortQoS=0x00;
	static const int AssuredQoS=0x02;
	static const int ReservedQoS=0x04;
	static const int GuaranteedQoS=0x08;
	static const int DefaultQoSLevel=BestEffortQoS;
public:
	SpaceWireRTQoSLevel();
	SpaceWireRTQoSLevel(int newleveltype)  throw(SpaceWireRTException);
	~SpaceWireRTQoSLevel();
private:
	bool isValidLevelType(int newleveltype);
public:
	int getQoSLevel();
	void setQoSLevel(int newleveltype) throw(SpaceWireRTException);
public:
	bool isBestEffortQoS(){
		if(leveltype==BestEffortQoS){
			return true;
		}else{
			false;
		}
	}
	bool isAssuredQoS(){
		if(leveltype==AssuredQoS){
			return true;
		}else{
			false;
		}
	}
	bool isReservedQoS(){
		if(leveltype==BestReservedQoS){
			return true;
		}else{
			false;
		}
	}
	bool isGuaranteedQoS(){
		if(leveltype==GuaranteedQoS){
			return true;
		}else{
			false;
		}

	}
};

#endif /*SPACEWIRERTQOSLEVEL_HH_*/

/** History
 * 2008-08-26 file created (Takayuki Yuasa)
 */