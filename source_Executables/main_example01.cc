#include "SpaceWire.hh"
#include "RMAP.hh"

#include "Thread.hh"
#include "Mutex.hh"
#include "Condition.hh"

#include <iostream>
using namespace std;

class ChildClassOfThread : public Thread {
private:
	int id;
	Condition* condition;
	Mutex* mutex;
public:
	ChildClassOfThread(int newid,Condition* newcondition,Mutex* newmutex)
		: Thread(),id(newid),condition(newcondition),mutex(newmutex){}
public:
	virtual void run(){
		for(unsigned int i=0;i<5;i++){
			mutex->lock();
			cout << "ChildClassOfThread : " << id << endl;
			mutex->unlock();
			this->sleep(1000);
		}
		
		//wait until "signalled"
		mutex->lock();
		cout << "ChildClassOfThread : " << id << " waiting for signal" << endl;
		mutex->unlock();
		condition->wait();
		
		//show completion message
		mutex->lock();
		cout << "ChildClassOfThread : " << id << " signalled" << endl;
		mutex->unlock();
		
		this->exit();
	}
};

class Main {
private:
	Condition condition;
	Mutex mutex;
public:
	Main(){
		//Initialization
	}
	void run(){
		//Instantiate ChildClassOfThread class 
		ChildClassOfThread thread1(1,&condition,&mutex);
		ChildClassOfThread thread2(2,&condition,&mutex);
		
		//start threads
		mutex.lock();
		int status1=thread1.start();
		int status2=thread2.start();
		cout << "thread1 started : " << status1 << endl;
		cout << "thread2 started : " << status2 << endl;
		mutex.unlock();
		
		//wait 10sec
		condition.wait(10000);
		
		//signalling
		condition.broadcast();
		
		//wait 1sec more
		condition.wait(1000);
		
		//then, quit
		mutex.lock();
		cout << "Main completed" << endl;
		mutex.unlock();
	}
};

int main(int argn,char* argv[]){
	Main main;
	main.run();
	return 0;
}

/** History
 * 2008-05-xx file created (Takayuki Yuasa)
 */
