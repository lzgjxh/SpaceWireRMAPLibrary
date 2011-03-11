#ifndef IPSERVERSOCKET_HH_
#define IPSERVERSOCKET_HH_

#include "IPSocket.hh"

using namespace std;

class IPServerAcceptedSocket : public IPSocket {
public:
	IPServerAcceptedSocket();
	~IPServerAcceptedSocket();
	void open() throw(IPSocketException);
	void close();
	bool isServerSocket();
public:
	void setAddress(struct ::sockaddr_in* newaddress);
private:
	struct ::sockaddr_in address;
};

class IPServerSocket : public IPSocket {
public:
	IPServerSocket(int newport);
	~IPServerSocket();
	void open() throw(IPSocketException);
	void close();
	bool isServerSocket();
public:
	void create() throw(IPSocketException);
	void bind() throw(IPSocketException);
	void listen() throw(IPSocketException);
	IPServerAcceptedSocket* accept() throw(IPSocketException);
private:
	static const int maxofconnections=5;
};

#endif /*IPSERVERSOCKET_HH_*/
