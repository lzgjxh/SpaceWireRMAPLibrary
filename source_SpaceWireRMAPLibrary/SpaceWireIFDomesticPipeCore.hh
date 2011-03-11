#ifndef SPACEWIREIFDOMESTICPIPECORE_HH_
#define SPACEWIREIFDOMESTICPIPECORE_HH_

class SpaceWireIFDomesticPipeCore;

#include "SpaceWireIFDomesticPipe.hh"

#include "Mutex.hh"
#include "Condition.hh"

#include <queue>
#include <vector>
using namespace std;

class SpaceWireIFDomesticPipeCore {
private:
	Mutex mutexAtoB;
	Mutex mutexBtoA;
	queue< vector<unsigned char>* > bufferAtoB;
	queue< vector<unsigned char>* > bufferBtoA;

	SpaceWireIFDomesticPipe* nodea;
	SpaceWireIFDomesticPipe* nodeb;
private:
	unsigned char timecode;
public:
	SpaceWireIFDomesticPipeCore();
	~SpaceWireIFDomesticPipeCore();
	SpaceWireIFDomesticPipe* getPipeEndA();
	SpaceWireIFDomesticPipe* getPipeEndB();
	void pushPacket(unsigned int pipeendtype,vector<unsigned char>* packet);
	vector<unsigned char> popPacket(unsigned int pipeendtype);
	void setTimeCode(unsigned char newtimecode);
	unsigned char getTimeCode();
public:
	static const unsigned int DomesticPipeEndTypeA=0x00;
	static const unsigned int DomesticPipeEndTypeB=0x01;
};

#endif /*SPACEWIREIFDOMESTICPIPECORE_HH_*/

/** History
 * 2008-09-xx file created (Takayuki Yuasa)
 * 2008-12-17 added TimeCode related methods (Takayuki Yuasa)
 */
