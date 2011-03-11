#include "SpaceWireIFDomesticPipeCore.hh"
#include "SpaceWireIFDomesticPipe.hh"
#include "SpaceWireRT.hh"

#include "Thread.hh"
#include "Mutex.hh"

#include <iostream>
#include <string>
using namespace std;

Mutex coutmutex;

class NodeThread : public Thread {
private:
	SpaceWireIF* spacewireif;
	SpaceWireRTDestination* rtdestinationtopeer;
	string name;
public:
	NodeThread(string newname,SpaceWireIF* newspacewireif,SpaceWireRTDestination* newdestination) : name(newname), spacewireif(newspacewireif), rtdestinationtopeer(newdestination) {}
public:
	void run(){
		coutmutex.lock();cout << name << " initializing SpaceWireRTEngine..." << endl;coutmutex.unlock();
		SpaceWireRTEngine* rtengine=new SpaceWireRTEngine(spacewireif);
		rtengine->start();

		cout << "creating SpaceWireRTChannel to the destination..." << endl;
		SpaceWireRTChannel* channletopeer=rtengine->createChannel(rtdestinationtopeer);
		channeltopeer->send()

	}
};

class Main {
public:
	Main(){
		//Initialization
	}
public:
	void run(){
		//initialization
		cout << "initializing SpaceWireIF..." << endl;
		SpaceWireIFDomesticPipeCore* spacewiredomesticpipecore=new SpaceWireIFDomesticPipeCore();
		SpaceWireIF* spacewireifa=spacewiredomesticpipecore->getPipeEndA();
		spacewireifa->initialize();
		spacewireifa->open();
		SpaceWireIF* spacewireifb=spacewiredomesticpipecore->getPipeEndB();
		spacewireifb->initialize();
		spacewireifb->open();
		
		//create threads
		NodeThread nodea("nodea", spacewireifa);
		NodeThread nodeb("nodeb", spacewireifb);
		nodea.start();
		nodeb.start();
		
		
		
		cout << "initializing SpaceWire I/F..." << endl;
		
		
		
		
		//finalization
		rmapengine->closeRMAPSocket(rmapsocket);
		rmapengine->stop();
		spacewireif->close();
		delete rmapengine;
		delete spacewireif;				
	}
};

int main(int argc,char* argv[]){
	Main main;
	main.run();
	return 0;
}
