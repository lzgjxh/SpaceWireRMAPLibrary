#ifndef MESSAGE_HH_
#define MESSAGE_HH_

#include <iostream>
#include <sys/types.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <queue>

#include "Mutex.hh"
#include "Condition.hh"
using namespace std;

class Message {
private:
	queue<string*> messagequeue;
	Mutex messagequeuemutex;
	Condition condition;
public:
	Message();
	~Message();
	void send(string message) throw (SynchronousException);
	string receive() throw (SynchronousException);
	string receive(int timeoutInMillisecond) throw (SynchronousException);
public:
	static const unsigned int BufferSize=256;
	static const int DepthOfMessageQueue=1024;
private:
	static const int WaitInterval=10; //10ms
};



#endif /* MESSAGE_HH_ */

/** History
 * 2008-09-xx file created (Toshinari Hagihara)
 * 2008-10-03 restructuring (Takayuki Yuasa)
 */
