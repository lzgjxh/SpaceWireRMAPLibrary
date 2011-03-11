/** Sample program for Multiple RMAP Transaction with
 * multiple SpaceWire ports of SpaceCube1. If the SpaceWire
 * driver supports the use (open) of SpaceWire IFs from
 * multiple threads(task), this program will successfully
 * perform read access to two RMAPDestinations simultaneously
 * showing messages with the number of the currently running
 * thread.
 */

#include "SpaceWire.hh"
#include "RMAP.hh"

#include "Thread.hh"
#include "Mutex.hh"
#include "Condition.hh"

#include <iostream>
using namespace std;

class Process : public Thread {
private:
	SpaceWireIF* spacewireif;
	RMAPEngine* rmapengine;
	RMAPSocket* rmapsocket;
	RMAPDestination rmapdestination;
	int portnumber;
	Mutex* mutex;
public:
	Process(int newportnumber,RMAPDestination newrmapdestination,Mutex* newmutex) : portnumber(newportnumber),rmapdestination(newrmapdestination),mutex(newmutex) {
		spacewireif=SpaceWireIFImplementations::selectInstanceFromCommandLineMenu();
		spacewireif->open(portnumber);
		rmapengine=new RMAPEngine(spacewireif);
		rmapdestination.setDestinationKey(0x02);
		rmapsocket=rmapengine->openRMAPSocketTo(rmapdestination);
	}
	~Process(){
		rmapengine->closeRMAPSocket(rmapsocket);
		delete spacewireif;
		delete rmapengine;
	}
	void run(){
		for(int i=0;i<10;i++){
			vector<unsigned char> data=*(rmapsocket->read(0x00000000,100));
			mutex->lock();
			cout << "Process port:" << portnumber << " done" << endl;
			mutex->unlock();
			sleep(1000);
		}
	}
};

int main(int argc,char* argv[]){
	RMAPDestination rmapdestination1;
	rmapdestination1.setDestinationKey(0x02);
	rmapdestination1.setDestinationLogicalAddress(0x01);
	RMAPDestination rmapdestination2;
	rmapdestination2.setDestinationKey(0x02);
	rmapdestination2.setDestinationLogicalAddress(0x01);
	Mutex mutex;
	
	Process process_port1(1,rmapdestination1,&mutex);
	Process process_port2(0,rmapdestination2,&mutex);
	
	//start threads
	process_port1.start();
	process_port2.start();
}
