#ifndef THREAD_HH_
#define THREAD_HH_

#include "Condition.hh"
#include "Mutex.hh"
#include "SynchronousException.hh"

#include <iostream>
#include <iomanip>
#include <basic.h>
#include <btron/proctask.h>
#include <exception>
#include <map>
using namespace std;

/** Thread class which wraps multi-task implementation of
 * each operating systems so that users easily write
 * multi thread program in C++. This class is especially
 * coded to capusulate T-Kernel task-related system calls.
 * Likewise Java Thread, run() method should be overridden
 * in inheriting child class to perform user defined routine.
 * Sleep and interrupt are available.
 */
class Thread{
private:
	static void start_routine(W newindex);
	static Mutex arraymutex;
	static map<unsigned int,Thread*> array;
	static int index;
	static int activethreads;
	static bool initialized;
	static const int MaximumThreadNumber=16;
	bool started;
public:
	/** Constructor.
	 */
	Thread();
	
	/** Destructor.
	 */
	virtual ~Thread() =0;
	
	/** Starts running this thread.
	 */
	int start() throw (SynchronousException);
	
	/** Pre-empts CPU resources from this thread.
	 */
	void yield();
	
	/** Sleeps. By calling interrupt() method from
	 * other thread, users can wake up the sleeping
	 * thread.
	 * @param millisecond sleep duration
	 */
	void sleep(double millisecond);
	
	/** Waits forever.
	 * interrupt() can wake up the waiting thread.
	 */
	void wait();
	
	/** Interruputs waiting or sleeping.
	 * If this thread is waiting/sleeping, by calling
	 * this method from other thread, users can wake
	 * up this thread.
	 */
	void interrupt();
	
	/** Destroy this thread.
	 */
	void exit();

	/**
	 */
	void notify();
	
	/**
	 */
	int detach();
	/**
	 */
	int join();

protected:
	/** A method in which thread process should be coded
	 * in child classes.
	 */
	virtual void run() =0;
protected:
	W threadid;
	Condition condition;
	Condition joincondition;
};

#endif /*THREAD_HH_*/
