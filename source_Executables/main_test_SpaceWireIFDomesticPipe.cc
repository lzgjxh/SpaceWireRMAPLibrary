#include "SpaceWireIFDomesticPipe.hh"
#include "SpaceWireIFDomesticPipeCore.hh"

#include "Condition.hh"
#include "Mutex.hh"
#include "Thread.hh"

#include <iostream>
#include <iomanip>
using namespace std;

Mutex coutmutex;

class PipeThread : public Thread {
private:
	SpaceWireIF* spacewireif;
	string name;
public:
	PipeThread(string newname,SpaceWireIFDomesticPipe* newspacewireif) : spacewireif(newspacewireif), name(newname){}
	void run(){
		vector<unsigned char> packet;
		//send 10 packets
		for(unsigned char i=0;i<10;i++){
			packet.push_back(i);
			coutmutex.lock();
			cout << name << " sending packet no." << (unsigned int)i << endl;
			coutmutex.unlock();
			spacewireif->send(&packet);
		}
		//receive 10 packets
		for(unsigned char i=0;i<10;i++){
			packet=spacewireif->receive();
			coutmutex.lock();
			cout << name << " received packet no." << (unsigned int)i << " size=" << packet.size() << endl;
			SpaceWireUtilities::dumpPacket(&packet);
			coutmutex.unlock();
		}
		coutmutex.lock();
		cout << name << "finishing..." << endl;
		coutmutex.unlock();
	}
};

class Main {
public:
	Main(){
		//Initialization
	}
public:
	void run(){
		//User Process
		SpaceWireIFDomesticPipeCore domesticpipecore;
		
		PipeThread nodea("nodeA",domesticpipecore.getPipeEndA());
		PipeThread nodeb("nodeB",domesticpipecore.getPipeEndB());
		
		nodea.start();
		nodeb.start();
		
		coutmutex.lock();
		cout << "main thread waits 10sec for the completion of child-threads execution..." << endl;
		coutmutex.unlock();
		Condition condition;
		condition.wait(10000);
	}
};

int main(int argc,char* argv[]){
	Main main;
	main.run();
	return 0;
}
