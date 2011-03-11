#include "Condition.hh"
#include "Message.hh"
#include "Mutex.hh"
#include "Thread.hh"

#include <iostream>
using namespace std;

Mutex coutmutex;

class ThreadA : public Thread {
private:
	Message* message;
public:
	ThreadA(Message* newmessage) : message(newmessage){};
	void run(){
		try{
			coutmutex.lock();
			cout << "ThreadA sending" << endl;
			coutmutex.unlock();
			
			message->send("Message from ThreadA to ThreadB!");
	
			coutmutex.lock();
			cout << "ThreadA sending done" << endl;
			coutmutex.unlock();
		}catch(SynchronousException e){
			e.dump();
		}
	}
};

class ThreadB : public Thread {
private:
	Message* message;
public:
	ThreadB(Message* newmessage) : message(newmessage){};
	void run(){
		try{
			coutmutex.lock();
			cout << "ThreadB received : " << message->receive() << endl;
			coutmutex.unlock();
		}catch(SynchronousException e){
			e.dump();
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
		try{
			Message* message=new Message();
			ThreadA* threada=new ThreadA(message);
			ThreadB* threadb=new ThreadB(message);
			threada->start();
			threadb->start();
			
			Condition condition;
			condition.wait(3000); //wait 3 seconds
			
			delete threada;
			delete threadb;
			delete message;
			
			cout << "Main::run() done" << endl;
		}catch(SynchronousException e){
			cout << "exception" << endl;
			e.dump();
		}
	}
};

int main(int argc,char* argv[]){
	Main main;
	main.run();
	return 0;
}

/** History
 * 2008-10-03 file created (Takayuki Yuasa)
  */
