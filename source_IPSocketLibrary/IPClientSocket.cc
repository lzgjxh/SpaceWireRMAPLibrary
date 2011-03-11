#include "IPClientSocket.hh"

#include <stdlib.h>
#include <string.h>
using namespace std;

IPClientSocket::IPClientSocket() : IPSocket() {
	setURL("");
}

IPClientSocket::IPClientSocket(string newurl,int newport) : IPSocket() {
	setURL(newurl);
	setPort(newport);
}

IPClientSocket::~IPClientSocket(){}

void IPClientSocket::open() throw(IPSocketException){
	if(url.length()==0){
		throw IPSocketException("IPClientSocket::open() exception");
	}
	try{
		create();
		connect();
	}catch(IPSocketException e){
		cout << "exception in IPClientSocket::open()" << endl;
		throw IPSocketException();
	}
}

void IPClientSocket::open(string newurl,int newport) throw(IPSocketException){
	setURL(newurl);
	setPort(newport);
	open();
}

void IPClientSocket::close(){
#ifdef SYSTEM_TYPE_POSIX
	::close(socketdescriptor);
#endif
#ifdef SYSTEM_TYPE_T_KERNEL
	::so_close(socketdescriptor);
#endif
	status=IPSocketInitialized;
}

bool IPClientSocket::isServerSocket(){
	return false;
}

void IPClientSocket::create() throw(IPSocketException){
#ifdef SYSTEM_TYPE_POSIX
	int result=::socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
#endif
#ifdef SYSTEM_TYPE_T_KERNEL
	int result=::so_socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
#endif
	if(result<0){
		throw IPSocketException("IPClientSocket::create() exception");
	}else{
		status=IPSocketCreated;
		socketdescriptor=result;
	}
}
void IPClientSocket::connect() throw(IPSocketException){
	int result;
	if(status==IPSocketConnected){
		return;
	}
	struct ::sockaddr_in serveraddress;
	memset(&serveraddress,0,sizeof(struct ::sockaddr_in));
	serveraddress.sin_family=AF_INET;
	serveraddress.sin_port=htons(getPort());
	//set url or ip address
	struct ::hostent* hostentry;
#ifdef SYSTEM_TYPE_POSIX
	hostentry=::gethostbyname(url.c_str());
	if(hostentry==NULL){
		throw IPSocketException();
	}else{
		serveraddress.sin_addr.s_addr=*((unsigned long*)hostentry->h_addr_list[0]);
	}
#endif
#ifdef SYSTEM_TYPE_T_KERNEL
	unsigned long ip;
	char temporal_buffer[HBUFLEN];

	ip=inet_addr(url.c_str());
	hostentry=(struct ::hostent*)malloc(sizeof(struct ::hostent));
	if((long)ip==-1){
		::so_gethostbyname((char*)url.c_str(),hostentry,temporal_buffer);
		if(hostentry==NULL){
			throw IPSocketException();
		}else{
			serveraddress.sin_addr.s_addr=*((unsigned long*)hostentry->h_addr_list[0]);
		}
	}else{
		serveraddress.sin_addr.s_addr=ip;
	}
#endif
	
#ifdef SYSTEM_TYPE_POSIX
	result=::connect(socketdescriptor,(struct ::sockaddr*)&serveraddress,sizeof(struct ::sockaddr_in));
#endif
#ifdef SYSTEM_TYPE_T_KERNEL
	result=::so_connect(socketdescriptor,(struct ::sockaddr*)&serveraddress,sizeof(struct ::sockaddr_in));
#endif
	if(result<0){
#ifdef SYSTEM_TYPE_T_KERNEL
		string errormessage;
		switch(result){
		case EX_PAR: errormessage="EX_PAR";break;
		case EX_ADR: errormessage="EX_ADR";break;
		case EX_BADF: errormessage="EX_BADF";break;
		case EX_HOSTUNREACH: errormessage="EX_HOSTUNREACH";break;
		case EX_INPROGRESS: errormessage="EX_INPROGRESS";break;
		case EX_TIMEDOUT: errormessage="EX_TIMEDOUT";break;
		case EX_CONNABORTED: errormessage="EX_CONNABORTED";break;
		}
		cout << errormessage << endl;
#endif
		throw IPSocketException("IPClientSocket::connect() exception");
	}else{
		status=IPSocketConnected;
	}

	//memcpy(&(serveraddress.sin_addr.s_addr),hostentry->h_addr_list[0],sizeof(unsigned int));
}

void IPClientSocket::setURL(string newurl){
	url=newurl;
}

/** History
 * 2008-06-xx file created (Takayuki Yuasa)
 * 2008-08-25 bug fix (Toshinari Hagihara)
 */
