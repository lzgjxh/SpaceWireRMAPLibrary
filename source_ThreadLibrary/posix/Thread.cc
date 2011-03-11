//Thread.cpp
//This class wraps pthread function.

#include "Thread.hh"

#ifndef uintptr_t
#include <stdint.h>
#endif

using namespace std;

int Thread::test=111;

Thread::Thread() {
	threadid=0;
}

Thread::~Thread() {
}

void* Thread::start_routine(void* arg) {
	if(arg==0){
		cout << "Thread start_routine() returning zero" << hex << (uintptr_t)(arg) << endl;cout.flush();
		return 0;
	}
	Thread* newthread = reinterpret_cast<Thread*>(arg);
	newthread->run();
	pthread_exit(arg);
	return arg;
}

int Thread::start() {
	int result=pthread_create(&threadid,NULL,Thread::start_routine,(void*)this);
	return result;
}

void Thread::yield() {
	sched_yield();
}

void Thread::sleep(double millis) {
	condition.wait(millis);
}

void Thread::wait() {
	condition.wait();
}

void Thread::interrupt() {
	condition.signal();
}

void Thread::notify() {
	condition.signal();
	yield();
}

int Thread::join() {
	return pthread_join(threadid,0);
}

void Thread::exit(){
	pthread_exit((void**)0);
}

int Thread::detach(){
	return pthread_detach(threadid);
}

int Thread::cancel(){
	return pthread_cancel(threadid);
}

int Thread::kill(int signo){
	return pthread_kill(threadid, signo);
}
