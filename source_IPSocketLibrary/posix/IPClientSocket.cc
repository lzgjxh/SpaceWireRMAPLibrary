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
	::close(socketdescriptor);
	status=IPSocketInitialized;
}

bool IPClientSocket::isServerSocket(){
	return false;
}

void IPClientSocket::create() throw(IPSocketException){
	int result=::socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
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
	hostentry=::gethostbyname(url.c_str());
	if(hostentry==NULL){
		throw IPSocketException();
	}else{
		serveraddress.sin_addr.s_addr=*((unsigned long*)hostentry->h_addr_list[0]);
	}
	result=::connect(socketdescriptor,(struct ::sockaddr*)&serveraddress,sizeof(struct ::sockaddr_in));
	if(result<0){
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
