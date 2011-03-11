#ifndef MUTEX_HH_
#define MUTEX_HH_

#include "SynchronousException.hh"

#include <iostream>
#include <btron/tkcall.h>
#include <tk/typedef.h>
using namespace std;

/** Mutex (mutual-exclusion) class which is used
 * to synchronize multiple threads like "synchronize"
 * keyword in Java.
 */
class Mutex {
private:
	ID mutexid;
	T_CMTX mutex_attr;
public:
	/** Constructor.
	 */
	Mutex();
	
	/** Destructor.
	 */
	~Mutex();
	
	/** Locks the mutex. By calling this method,
	 * a thread can enter a critical section.
	 */
	void lock();
	
	/** Unlocks the mutex. By calling this method,
	 * a thread exit from a critical section.
	 */ 
	void unlock();
	
	/** Getter for mutex id.
	 * @return mutex id
	 */
	ID getMutexID();
};

#endif /*MUTEX_HH_*/
