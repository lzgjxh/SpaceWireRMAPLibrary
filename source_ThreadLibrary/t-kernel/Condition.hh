#ifndef CONDITION_HH_
#define CONDITION_HH_

#include "Mutex.hh"

#include <btron/tkcall.h>
#include <tk/typedef.h>
using namespace std;

/** A class which is used to synchronize multiple
 * threads. Thsi implementation is based on T-Kernel's
 * Event Flag mechanism.
 */
class Condition {
private:
	ID eventflagid;
	T_CFLG eventflag_attr;
public:
	/** Constructor.
	 */
	Condition();
	
	/** Destructor.
	 */
	~Condition();
	
	/** Waits until signalled.
	 */
	void wait();
	
	/** Waits until signalled or time-outed.
	 * @param millis time out duration in milli second
	 */
	void wait(unsigned int millis);
	
	/** Tell signal to a waiting thread.
	 */
	void signal();
	
	/** Tell signal to all waiting threads.
	 */
	void broadcast();
};

#endif /*CONDITION_HH_*/
