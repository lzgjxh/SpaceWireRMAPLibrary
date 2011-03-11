#include "SpaceWire.hh"
#include "RMAP.hh"

#include "Thread.hh"
#include "Condition.hh"
#include "Mutex.hh"

#include <iostream>
#include <vector>
using namespace std;

int Loop=0;
Condition condition;
Mutex coutmutex;

class SendThread : public Thread {
private:
	SpaceWireIF* spacewireif;
public:
	SendThread(SpaceWireIF* newspacewireif) : Thread(),spacewireif(newspacewireif) {
		
	}
	void run(){
		vector<unsigned char>* data=new vector<unsigned char>();
		data->push_back(0x05);
		data->push_back(0xfe);
		for(int i=0;i<40000;i++){
			data->push_back(i);
		}
		int loopcount=0;
		while(Loop==1){
			try{
				coutmutex.lock();cout << "sending " << data->size() << "bytes" << endl;coutmutex.unlock();
				spacewireif->send(data);
			}catch(SpaceWireException e){
				coutmutex.lock();
				cout << "SendThread" << endl;
				e.dump();
				coutmutex.unlock();
			}
		}
	}
};

class ReceiveThread : public Thread {
private:
	SpaceWireIF* spacewireif;
public:
	ReceiveThread(SpaceWireIF* newspacewireif) : Thread(),spacewireif(newspacewireif) {
		
	}
	void run(){
		vector<unsigned char>* data=new vector<unsigned char>();
		int loopcount=0;
		while(Loop==1){
			try{
				spacewireif->receive(data);
				coutmutex.lock();cout << "received " << data->size() << "bytes" << endl;coutmutex.unlock();
			}catch(SpaceWireException e){
				coutmutex.lock();
				cout << "ReceiveThread" << endl;
				e.dump();
				coutmutex.unlock();
			}
		}
	}
};

class Main {
public:
	Main(){
		//Initialization
	}
public:
	void run(){
		cout << "SpaceCube Router IP Core Test (Circulation)" << endl;
		cout << " Note : This test program requires a SpaceWire connection between " << endl;
		cout << "        SpaceCube Router Port 1 (CN2) and Port 2 (CN3). Ensure that" << endl;
		cout << "        they are connected with a SpaceWire cable." << endl;
		cout << endl;
		
		cout << "#################################" << endl;
		cout << " Initialize" << endl;
		cout << "#################################" << endl;
		vector<unsigned char>* sentdata=new vector<unsigned char>();
		vector<unsigned char>* receiveddata=new vector<unsigned char>();
		
		try{
		cout << "Opening SpaceWire I/F Port 4..." << endl;
		SpaceWireIF* spacewireif4=new SpaceWireIFSpaceCube1ShimafujiRouterIPcore();//open port 4
		spacewireif4->initialize();
		spacewireif4->open(4);
		SpaceWireLinkStatus ls4=spacewireif4->getLinkStatus();
		ls4.setTimeout(1000);
		spacewireif4->setLinkStatus(ls4);
		cout << "done" << endl << endl;
		
		cout << "Opening SpaceWire I/F Port 5..." << endl;
		SpaceWireIF* spacewireif5=new SpaceWireIFSpaceCube1ShimafujiRouterIPcore();//open port 5
		spacewireif5->initialize();
		spacewireif5->open(5);
		SpaceWireLinkStatus ls5=spacewireif5->getLinkStatus();
		ls5.setTimeout(1000);
		spacewireif5->setLinkStatus(ls5);
		cout << "done" << endl << endl;

		
		SendThread* sendthread=new SendThread(spacewireif4);
		ReceiveThread* receivethread=new ReceiveThread(spacewireif5);
		sendthread->start();
		receivethread->start();
		
		condition.wait(10000); //wait until receive done
		Loop=0;
		
		condition.wait(10);
		cout << "#################################" << endl;
		cout << " Finalizatopn" << endl;
		cout << "#################################" << endl;
		cout << "Closing SpaceWire I/Fs" << endl;
		condition.wait(500);

		//finalization
		spacewireif4->close();
		delete spacewireif4;
		spacewireif5->close();
		delete spacewireif5;
		}catch(SpaceWireException e){
			e.dump();
			::exit(-100);
		}
	}
};

int main(int argc,char* argv[]){
	Loop=1;

	Main main;
	main.run();
	return 0;
}

/** History
 * 2008-10-04 file created (Takayuki Yuasa)
 */
