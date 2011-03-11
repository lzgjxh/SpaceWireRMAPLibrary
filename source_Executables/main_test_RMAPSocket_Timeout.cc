#include "RMAPEngine.hh"
#include "SpaceWireIFDomesticPipe.hh"
#include "SpaceWireIFDomesticPipeCore.hh"

#include "Condition.hh"
#include "Mutex.hh"
#include "Thread.hh"

#include <iostream>
#include <iomanip>
using namespace std;

Mutex coutmutex;

class StatusMonitorThread : public Thread {
private:
	RMAPEngine* rmapengine;
public:
	StatusMonitorThread(RMAPEngine* newrmapengine) : rmapengine(newrmapengine){}
	void run(){
		Condition condition;
		while(true){
			try{
				condition.wait(1000);
			}catch(SynchronousException e){
				cout << "Synchronous exception..." << endl;
				e.dump();
			}
			rmapengine->dump();
		}
	}
};

class PipeThread : public Thread {
private:
	SpaceWireIF* spacewireif;
	string name;
public:
	PipeThread(string newname,SpaceWireIFDomesticPipe* newspacewireif) : spacewireif(newspacewireif), name(newname){}
	void run(){
		vector<unsigned char> packet;
		//receive a packet
		packet=spacewireif->receive();
		coutmutex.lock();
		cout << name << " received packet:" << endl;
		SpaceWireUtilities::dumpPacket(&packet);
		cout << "  nodeB does not reply to this RMAP Command packet, so that" << endl;
		cout << "  nodeA's RMAPSocket will time out and throw exception." << endl;
		coutmutex.unlock();
		try{
			Condition condition;
			condition.wait(5000);
		}catch(SynchronousException e){
			cout << "Synchronous exception..." << endl;
			e.dump();
		}
	}
};

class Main {
private:
	unsigned int timeoutdurationInSecond;
public:
	Main(){
		//Initialization
		timeoutdurationInSecond=3; //time out in 3sec
	}
public:
	void run(){
		//User Process
		SpaceWireIFDomesticPipeCore domesticpipecore;
		SpaceWireIF* spacewireif=domesticpipecore.getPipeEndA();
		RMAPEngine* rmapengine=new RMAPEngine(spacewireif);
		rmapengine->start();
		
		RMAPDestination rmapdestination; 
		RMAPSocket* socket=rmapengine->openRMAPSocketTo(rmapdestination);
		socket->enableTimeout(timeoutdurationInSecond*1000);
		
		PipeThread nodeb("nodeB",domesticpipecore.getPipeEndB());
		nodeb.start();
		
		StatusMonitorThread monitor(rmapengine);
		monitor.start();
		
		try{
			cout << "nodeA RMAPSocket read...(with " << timeoutdurationInSecond << "sec timeout)" << endl;
			socket->read(0x1010,10);
		}catch(RMAPException e){
			coutmutex.lock();
			cout << "nodeA RMAPSocket exception..." << endl;
			e.dump();
			cout << "  RMAP Reply Wait will be automatically cancelled according to" << endl;
			cout << "  this TimedOut exception in RMAPEngine::receiveReply() method." << endl;
			coutmutex.unlock();
		}
		
		Condition condition;
		condition.wait(3000);
		coutmutex.lock();
		cout << "finishing main thread" << endl;
		coutmutex.unlock();
	}
};

int main(int argc,char* argv[]){
	Main main;
	main.run();
	return 0;
}
