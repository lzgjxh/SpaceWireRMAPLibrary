/** Sample Program for ThreadLibrary (Thread.cc, Condition.cc, and Mutex.cc)
 */

#include <iostream>
#include "Thread.hh"

using namespace std;

Mutex coutmutex;

/** Synchronous "cout"
 */
void synchronouscout(string str){
	coutmutex.lock();
	cout << str << endl;cout.flush();
	coutmutex.unlock();
}


class Request : public Thread {
public:
	Request(Condition* newcondition,Mutex* newmutex) : Thread(),condition(newcondition),mutex(newmutex){
		synchronouscout("class Request constructed");
	};
	virtual void run(){
		synchronouscout("Request run() invoked");
		while(true){
			synchronouscout("Request waiting for being signaled from other thread...");
			condition->wait();
			synchronouscout("Request waiting completed");
			sleep(1500);
		}
	}
private:
	Condition* condition;
	Mutex* mutex;
};

class Reply: public Thread {
public:
	Reply(Condition* newcondition,Mutex* newmutex) : Thread(),condition(newcondition),mutex(newmutex){
		synchronouscout("class Reply constructed");
	};
	virtual void run(){
		synchronouscout("Reply run() invoked");
		while(true){
			sleep(2000);
			synchronouscout("Reply signaling...");
			condition->signal();
			synchronouscout("Reply singaling completed");
		}
	}
private:
	Condition* condition;
	Mutex* mutex;
};

class Main {
private:
	Condition condition;
	Mutex mutex;
public:
	Main(){
		cout << "class Main constructed" << endl;cout.flush();
		//start threads
		Request request(&condition,&mutex);
		Reply reply(&condition,&mutex);
		coutmutex.lock();
		cout << "thread1 started : " << request.start() << endl;cout.flush();
		cout << "thread2 started : " << reply.start() << endl;cout.flush();
		coutmutex.unlock();
		//join (wait for joining those threads)
		request.join();
		reply.join();
	}
};

int main(int argn,char* argv[]){
	cout << "main() invoked" << endl;
	Main main;
	return 0;
}

/** History
 * 20080430 Takayuki Yuasa
 */
