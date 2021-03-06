#ifndef IPCLIENTSOCKET_HH_
#define IPCLIENTSOCKET_HH_

#include "IPSocket.hh"

#include "btron/errcode.h"

using namespace std;

class IPClientSocket : public IPSocket {
public:
	IPClientSocket();
	IPClientSocket(string newurl,int newport);
	~IPClientSocket();
	void open() throw(IPSocketException);
	void open(string newurl,int newport) throw(IPSocketException);
	void close();
	bool isServerSocket();
	void setURL(string newurl);
public:
	void create() throw(IPSocketException);
	void connect() throw(IPSocketException);
private:
	string url;
	struct ::sockaddr_in serveraddress;
	char temporal_buffer[HBUFLEN];
};

#endif /*IPCLIENTSOCKET_HH_*/
