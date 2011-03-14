#include "IPSocket.hh"

#include <sys/types.h>
#include <errno.h>

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
	int result=::so_send(socketdescriptor,(char*)data,length,0);
	if(result<0){
		throw IPSocketException("IPSocket::send() exception");
	}
	return result;
}

int IPSocket::receive(void* data,unsigned int length) throw(IPSocketException){
	int result=::so_recv(socketdescriptor,(char*)data,length,0);
	if(result<=0){
		cout << "TCP/IP Connection closed" << endl;
		throw IPSocketException("IPSocket::receive() connection closed");
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
}
