#ifndef IPSOCKET_HH_
#define IPSOCKET_HH_

#include "IPSocketException.hh"

#ifdef SYSTEM_TYPE_POSIX
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

#ifdef SYSTEM_TYPE_T_KERNEL
#include <btron/bsocket.h>
#endif

#include <iostream>
#include <string>
#include <unistd.h>
using namespace std;

class IPSocket {
public:
	enum IPSocketStatus {
		IPSocketInitialized,IPSocketCreated,IPSocketBound,IPSocketListening,IPSocketConnected
	};
public:
	IPSocket();
	virtual ~IPSocket();
	int getStatus();
	bool isConnected();
	virtual void open() throw(IPSocketException) =0;
	virtual void close() =0;
	int send(void* data,unsigned int length) throw(IPSocketException);
	int receive(void* data,unsigned int length) throw(IPSocketException);
	virtual bool isServerSocket() =0;
	void setSocketDescriptor(int newsocketdescriptor);
	void setPort(unsigned short newport);
	int getPort();
	void setName(string newname){
		name=newname;
	}
	string getName(){
		return name;
	}
	void setTimeout(unsigned int durationInMilliSec);
protected:
	int status;
private:
	string name;
	int timeoutDurationInMilliSec;
protected:
	int socketdescriptor;
	int port;

};

#endif /*IPSOCKET_HH_*/
