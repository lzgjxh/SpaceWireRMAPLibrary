#include "IPServerSocket.hh"

#ifdef SYSTEM_TYPE_T_KERNEL
#include "btron/errcode.h"
#endif

#include <string.h>
using namespace std;

///////////////////////////////////
//IPServerSocket
///////////////////////////////////
IPServerSocket::IPServerSocket(int newport) : IPSocket() {
	setPort(newport);
}

IPServerSocket::~IPServerSocket(){}

void IPServerSocket::open() throw(IPSocketException){
	if(getPort()<0){
		throw IPSocketException("IPServerSocket::open() exception");
	}
	create();
	bind();
	listen();
}

void IPServerSocket::close(){
	::so_close(socketdescriptor);
	status=IPSocketInitialized;
}

bool IPServerSocket::isServerSocket(){
	return true;
}

void IPServerSocket::create() throw(IPSocketException){
	int result=::so_socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(result<0){
		throw IPSocketException("IPServerSocket::create() exception");
	}else{
		status=IPSocketCreated;
		socketdescriptor=result;
	}
}

void IPServerSocket::bind() throw(IPSocketException){
	struct ::sockaddr_in serveraddress;
	memset(&serveraddress,0,sizeof(struct ::sockaddr_in));
	serveraddress.sin_family=AF_INET;
	serveraddress.sin_addr.s_addr=htonl(INADDR_ANY);
	serveraddress.sin_port=htons(getPort());
	int yes=1;
	int result=::so_bind(socketdescriptor,(struct ::sockaddr*)&serveraddress,sizeof(struct ::sockaddr_in));
	if(result<0){
		throw IPSocketException("IPServerSocket::bind() exception");
	}else{
		status=IPSocketBound;
	}
}

void IPServerSocket::listen() throw(IPSocketException){
	int result=::so_listen(socketdescriptor,maxofconnections);
	if(result<0){
		throw IPSocketException("IPServerSocket::listen() exception");
	}else{
		status=IPSocketListening;
	}
}

IPServerAcceptedSocket* IPServerSocket::accept() throw(IPSocketException){
	struct ::sockaddr_in clientaddress;
	int length=sizeof(clientaddress);
	int result=::so_accept(socketdescriptor,(struct ::sockaddr*)&clientaddress,&length);
	if(result<0){
		cout << "result:" << (result>>16) << endl;
		cout << "sockaddress desc.:" << socketdescriptor << endl; 
		throw IPSocketException("IPServerSocket::accept() exception");
	}else{
		IPServerAcceptedSocket* acceptedsocket=new IPServerAcceptedSocket();
		acceptedsocket->setAddress(&clientaddress);
		acceptedsocket->setPort(getPort());
		acceptedsocket->setSocketDescriptor(result);
		return acceptedsocket;
	}
}

///////////////////////////////////
//IPServerAcceptedSocket
///////////////////////////////////
IPServerAcceptedSocket::IPServerAcceptedSocket() : IPSocket() {}

IPServerAcceptedSocket::~IPServerAcceptedSocket(){}

void IPServerAcceptedSocket::open() throw(IPSocketException){
	throw IPSocketException("IPServerAcceptedSocket::open() exception");
}

void IPServerAcceptedSocket::close(){
	::so_close(socketdescriptor);
	status=IPSocketInitialized;
}

bool IPServerAcceptedSocket::isServerSocket(){
	return false;
}

void IPServerAcceptedSocket::setAddress(struct ::sockaddr_in* newaddress){
	memcpy(&address,newaddress,sizeof(struct ::sockaddr_in));
}
