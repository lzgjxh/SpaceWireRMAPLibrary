#include "IPSocket.hh"

#ifdef SYSTEM_TYPE_POSIX
#include <sys/types.h>
#include <errno.h>
#endif

#include <stdlib.h>
#include <cmath>
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
		throw IPSocketException(IPSocketException::TCPSocketError);
	}
	return result;
}

int IPSocket::receive(void* data,unsigned int length) throw(IPSocketException){
	ssize_t result=::recv(socketdescriptor,data,length,0);
	if(result<=0){
		if(errno==EAGAIN || errno==EWOULDBLOCK){
			//cout << "IPSocket::receive() TimeOut" <<endl;
			throw IPSocketException(IPSocketException::Timeout);
		}else{
			string err;
			switch(errno){
			case EBADF:err="EBADF";break;
			case ECONNREFUSED:err="ECONNREFUSED";break;
			case EFAULT:err="EFAULT";break;
			case EINTR:err="EINTR";break;
			case EINVAL:err="EINVAL";break;
			case ENOMEM:err="ENOMEM";break;
			case ENOTCONN:err="ENOTCONN";break;
			case ENOTSOCK:err="ENOTSOCK";break;
			}
			cout << "TCP/IP Connection closed : " << err<< endl;
			usleep(1000000);
			throw IPSocketException(IPSocketException::Disconnected);
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

void IPSocket::setTimeout(double durationInMilliSec){
	timeoutDurationInMilliSec=durationInMilliSec;
	struct timeval tv;
	tv.tv_sec = (unsigned int)(floor(durationInMilliSec/1000.));
	tv.tv_usec = (int)((durationInMilliSec-floor(durationInMilliSec))*1000);
	setsockopt(socketdescriptor, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof tv);

}

double IPSocket::getTimeoutDuration(){
	return timeoutDurationInMilliSec;
}
