#ifndef CONDITION_HH_
#define CONDITION_HH_

#include "Mutex.hh"

#include <pthread.h>
#include <sys/time.h>
using namespace std;

class Condition {
private:
	Mutex mutex;
	pthread_cond_t condition;
public:
	Condition();
	~Condition();
	void wait();
	void wait(int millis);
	void wait(unsigned int millis);
	void wait(double millis);
	void signal();
	void broadcast();
	Mutex* getConditionMutex();
};

#endif /*CONDITION_HH_*/
