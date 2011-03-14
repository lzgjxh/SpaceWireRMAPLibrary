#include "IPSocket.hh"

#ifdef SYSTEM_TYPE_POSIX
#include <sys/types.h>
#include <errno.h>
#endif

#include <stdlib.h>
using namespace std;

///////////////////////////////////
//IPSocket
///////////////////////////////////
IPSocket::IPSocket(){
	status=IPSocketInitialized;
	timeoutDurationInMilliSec=0;
	setPort(-1);
}

IPSocket::~IPSocket(){}

int IPSocket::getStatus(){
	return status;
}

bool IPSocket::isConnected(){
	if(status==IPSocketConnected){
		return true;
	}else{
		return false;
	}
}

int IPSocket::send(void* data,unsigned int length) throw(IPSocketException){
	int result=::send(socketdescriptor,data,length,0);
	if(result<0){
		throw IPSocketException("IPSocket::send() exception");
	}
	return result;
}

int IPSocket::receive(void* data,unsigned int length) throw(IPSocketException){
	ssize_t result=::recv(socketdescriptor,data,length,0);
	if(result<=0){
		if(errno==EAGAIN || errno==EWOULDBLOCK){
			//cout << "IPSocket::receive() TimeOut" <<endl;
			throw IPSocketException("IPSocket::receive() TimeOut");
		}else{
			cout << "TCP/IP Connection closed" << endl;
			throw IPSocketException("IPSocket::receive() connection closed");
			exit(-1);
		}
	}
	return result;
}

void IPSocket::setSocketDescriptor(int newsocketdescriptor){
	socketdescriptor=newsocketdescriptor;
}

void IPSocket::setPort(unsigned short newport){
	port=newport;
}

int IPSocket::getPort(){
	return port;
}

void IPSocket::setTimeout(unsigned int durationInMilliSec){
	timeoutDurationInMilliSec=durationInMilliSec;
	struct timeval tv;
	tv.tv_sec = (unsigned int)(durationInMilliSec/1000.);
	tv.tv_usec = (int)((durationInMilliSec/1000.)*1000);
	setsockopt(socketdescriptor, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof tv);
}
