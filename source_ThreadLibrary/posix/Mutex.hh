#ifndef MUTEX_HH_
#define MUTEX_HH_

#include "SynchronousException.hh"

#include <iostream>
#include <pthread.h>
using namespace std;

class Mutex {
private:
	pthread_mutex_t mutex;
	pthread_mutexattr_t mutex_attr;
public:
	Mutex();
	~Mutex();
	void lock();
	void unlock();
	pthread_mutex_t* getPthread_Mutex_T();
};

#endif /*MUTEX_HH_*/
