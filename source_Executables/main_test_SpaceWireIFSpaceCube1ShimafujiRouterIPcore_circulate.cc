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

//quit with key input
void task_swt( W tid_cam ){
	getchar();
	Loop = 0;
	condition.signal();
	// タスクを終了する 
	ext_tsk();
}

class SendThread : public Thread {
private:
	SpaceWireIF* spacewireif;
public:
	SendThread(SpaceWireIF* newspacewireif) : Thread(),spacewireif(newspacewireif) {
		
	}
	void run(){
		vector<unsigned char>* data=new vector<unsigned char>();
		data->push_back(0x01);
		data->push_back(0x05);
		data->push_back(0xfe);
		for(int i=0;i<2100;i++){
			data->push_back(i);
		}
		int loopcount=0;
		while(Loop==1){
			cout << "[" << endl;
			spacewireif->send(data);
			cout << "]" << endl;
			
			if(loopcount==100){
				cout << "100 send" << endl;
				loopcount=0;
			}else{
				loopcount++;
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
			cout << "(" << endl;
			spacewireif->receive(data);
			cout << ")" << endl;
			
			if(loopcount==100){
				cout << "100 receive" << endl;
				loopcount=0;
			}else{
				loopcount++;
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
		
		cout << "Opening SpaceWire I/F Port 4..." << endl;
		SpaceWireIF* spacewireif4=new SpaceWireIFSpaceCube1ShimafujiRouterIPcore();//open port 4
		spacewireif4->initialize();
		spacewireif4->open(4);
		cout << "done" << endl << endl;
		
		cout << "Opening SpaceWire I/F Port 5..." << endl;
		SpaceWireIF* spacewireif5=new SpaceWireIFSpaceCube1ShimafujiRouterIPcore();//open port 5
		spacewireif5->initialize();
		spacewireif5->open(5);
		cout << "done" << endl << endl;

		
		SendThread* sendthread=new SendThread(spacewireif4);
		ReceiveThread* receivethread=new ReceiveThread(spacewireif5);
		sendthread->start();
		receivethread->start();
		
		condition.wait();
		condition.wait(1000); //wait 1s
		
		cout << "#################################" << endl;
		cout << " Finalizatopn" << endl;
		cout << "#################################" << endl;
		cout << "Closing SpaceWire I/Fs" << endl;
		
		//finalization
		spacewireif4->close();
		delete spacewireif4;
		spacewireif5->close();
		delete spacewireif5;
	}
};

int main(int argc,char* argv[]){
	Loop=1;
	cre_tsk( task_swt, -1, get_tid() );		// キー入力で終了するようにタスク起動
	Main main;
	main.run();
	return 0;
}

/** History
 * 2008-10-04 file created (Takayuki Yuasa)
 */
