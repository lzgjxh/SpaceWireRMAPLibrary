#ifndef IPSOCKETEXCEPTION_HH_
#define IPSOCKETEXCEPTION_HH_

#include <exception>
#include <iostream>
#include <string>
using namespace std;

class IPSocketException{
private:
	int status;
public:
	enum{
		Disconnected,
		Timeout,
		TCPSocketError,
		PortNumberError,
		BindError,
		ListenError,
		AcceptException,
		OpenException,
		CreateException,
		HostEntryError,
		ConnectException,
		Undefied
	};
public:
	IPSocketException();
	IPSocketException(int status);
	void dump(){
		cout << "IPSocketException status=" << status << endl;
	}
	void setStatus(int status){
		this->status=status;
	}
	int getStatus(){
		return status;
	}
};

#endif /*IPSOCKETEXCEPTION_HH_*/
