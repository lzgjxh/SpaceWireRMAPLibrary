//Thread.cpp
//This class wraps pthread function.

#include "Thread.hh"

using namespace std;

//#define DEBUG

bool Thread::initialized=false;
int Thread::index=0;
map<unsigned int,Thread*> Thread::array;
int Thread::activethreads=0;
Mutex Thread::arraymutex;

Thread::Thread() {
	arraymutex.lock();
	if(initialized==false){
		initialized=true;
	}
	arraymutex.unlock();
	started=false;
}

Thread::~Thread() {
	arraymutex.lock();
	activethreads--;
	arraymutex.unlock();
}

void Thread::start_routine(W newindex) {
	arraymutex.lock();
	Thread* newthread=array.find(newindex)->second;
	arraymutex.unlock();
	newthread->run();
	newthread->exit();
}

int Thread::start() throw(SynchronousException){
#ifdef DEBUG
	cout << "Thread::start() invoked" << endl;
#endif
	started=true;
	arraymutex.lock();
	if(activethreads<MaximumThreadNumber){
		array.insert(pair<unsigned int,Thread*>(index,this));
		activethreads++;
	}else{
		cout << "Thread::start() exception (Too many Threads are running)" << endl;
		throw(SynchronousException("Too many Threads are running"));
	}
#ifdef DEBUG
	cout << "Thread::start() create task" << endl;
#endif
	threadid=cre_tsk((Thread::start_routine),-1,index);
#ifdef DEBUG
	cout << "Thread::start() create task completed" << endl;
#endif
	index++;
	arraymutex.unlock();
	return threadid;
}

void Thread::yield() {
}

void Thread::sleep(double millis) {
	if(millis>0){
		condition.wait((unsigned int)millis);
	}
}

void Thread::wait() {
	condition.wait();
}

void Thread::interrupt() {
	condition.signal();
}

void Thread::notify() {
}

int Thread::join() {
	joincondition.wait();
}

void Thread::exit(){
	joincondition.broadcast();
	return ext_tsk();
}

int Thread::detach(){
	exit();
}