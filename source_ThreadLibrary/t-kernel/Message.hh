#ifndef MESSAGE_HH_
#define MESSAGE_HH_

#include <iostream>
#include <sys/types.h>
#include <errno.h>

#include "Mutex.hh"
#include "SynchronousException.hh"

class Message {
private:
	ID msqid;
	char* sendmsgbuf;
	char* recvmsgbuf;
	T_CMBF cmbf;
	ER status;
	Mutex sendmutex;
	Mutex recvmutex;
public:
	Message();
	~Message();
	void send(string message) throw (SynchronousException);
	string receive() throw (SynchronousException);
	string receive(int timeoutInMillisecond) throw (SynchronousException);
public:
	static const int BufferSize=256;
	static const int DepthOfMessageQueue=1024;
};

#endif /* MESSAGE_HH_ */

/** History
 * 2008-09-xx file created (Toshinari Hagihara)
 * 2008-10-03 restructuring (Takayuki Yuasa)
 */