#ifndef THREAD_HH_
#define THREAD_HH_

#include "Condition.hh"
#include "Mutex.hh"
#include "SynchronousException.hh"

#include <iostream>
#include <iomanip>
#include <pthread.h>
#include <exception>
#ifndef __CINT__
#include <sys/wait.h>
#endif
using namespace std;


class Thread{
private:
	static void* start_routine(void *);
	static int test;
public:
	Thread();
	virtual ~Thread() =0;
	int start();
	void yield();
	void sleep(double millisecond);
	void wait();
	void interrupt();
	void notify();
	int detach();
	int join();
	int cancel();
	int kill(int signo);
	void exit();
protected:
	virtual void run() =0;
protected:
	pthread_t threadid;
	Condition condition;
};



/*
 * Reference : Java Thread major methods
 * 
 * static Thread currentThread();
 *  - returns reference to the current thread object.
 * 
 * void destroy();
 *  - destroy this thread object without clean up.
 * 
 * static void dumpStack();
 *  - dump the current thread stack trace.
 * 
 * int getPriority();
 *  - returns the priority of this thread.
 * 
 * ThreadGroup getThreadGroup();
 *  - returns the thread group which this thread belongs to.
 * 
 * void intrerrupt();
 *  - interrupt this thread.
 * 
 * static boolean interrupted();
 *  - check if this thread is interrupted.
 * 
 * boolean isAlive();
 *  - check if this thread is alive.
 * 
 * boolean isDaemon();
 *  - check if this thread is daemon thread.
 * 
 * void join();
 *  - wait for quitting the thread.
 * 
 * void run();
 *  - the process which is executed in this thread.
 * 
 * void setPriority(int nrePriority);
 *  - set the priority of this thread.
 * 
 * static void sleep(long millis);
 *  - stop executing the thread by ordered milli second.
 * 
 * void start();
 *  - start executing this thread.
 * 
 */

#endif /*THREAD_HH_*/

/** History
 * 2010-01-18 Snow Leopard support (Takayuki Yuasa)
 */
