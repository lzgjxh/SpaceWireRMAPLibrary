#include "SpaceWireIFDomesticPipeCore.hh"

SpaceWireIFDomesticPipeCore::SpaceWireIFDomesticPipeCore(){
	nodea=new SpaceWireIFDomesticPipe(this,SpaceWireIFDomesticPipeCore::DomesticPipeEndTypeA);
	nodeb=new SpaceWireIFDomesticPipe(this,SpaceWireIFDomesticPipeCore::DomesticPipeEndTypeB);
}

SpaceWireIFDomesticPipeCore::~SpaceWireIFDomesticPipeCore(){
	delete nodea;
	delete nodeb;
}

SpaceWireIFDomesticPipe* SpaceWireIFDomesticPipeCore::getPipeEndA(){
	return nodea;
}

SpaceWireIFDomesticPipe* SpaceWireIFDomesticPipeCore::getPipeEndB(){
	return nodeb;
}

void SpaceWireIFDomesticPipeCore::pushPacket(unsigned int pipeendtype,vector<unsigned char>* packet){
	vector<unsigned char>* newpacket=new vector<unsigned char>();
	*newpacket=*packet; //copy byte-to-byte
	if(pipeendtype==SpaceWireIFDomesticPipeCore::DomesticPipeEndTypeA){
		mutexAtoB.lock();
		bufferAtoB.push(newpacket);
		mutexAtoB.unlock();
	}else if(pipeendtype==SpaceWireIFDomesticPipeCore::DomesticPipeEndTypeB){
		mutexBtoA.lock();
		bufferBtoA.push(newpacket);
		mutexBtoA.unlock();
	}else{
		delete newpacket;
		return;
	}
}

vector<unsigned char> SpaceWireIFDomesticPipeCore::popPacket(unsigned int pipeendtype){
	if(pipeendtype==SpaceWireIFDomesticPipeCore::DomesticPipeEndTypeB){
		Condition condition;
		while(true){
			if(bufferAtoB.size()!=0){
				vector<unsigned char>* poppedpacket=bufferAtoB.front();
				mutexAtoB.lock();
				bufferAtoB.pop();
				mutexAtoB.unlock();
				vector<unsigned char> returnpacket=*poppedpacket; //copy byte-to-byte
				delete poppedpacket;
				return returnpacket;
			}
			condition.wait(100);
		}
	}else if(pipeendtype==SpaceWireIFDomesticPipeCore::DomesticPipeEndTypeA){
		Condition condition;
		while(true){
			if(bufferBtoA.size()!=0){
				vector<unsigned char>* poppedpacket=bufferBtoA.front();
				mutexBtoA.lock();
				bufferBtoA.pop();
				mutexBtoA.unlock();
				vector<unsigned char> returnpacket=*poppedpacket; //copy byte-to-byte
				delete poppedpacket;
				return returnpacket;
			}
			condition.wait(100);
		}
	}else{
		return vector<unsigned char>();
	}
}

void SpaceWireIFDomesticPipeCore::setTimeCode(unsigned char newtimecode){
	timecode=newtimecode;
}

unsigned char SpaceWireIFDomesticPipeCore::getTimeCode(){
	return timecode;
}

/** History
 * 2008-09-xx file created (Takayuki Yuasa)
 * 2008-12-17 added TimeCode related methods (Takayuki Yuasa)
 */
