#ifndef IPSOCKETEXCEPTION_HH_
#define IPSOCKETEXCEPTION_HH_

#include <exception>
#include <iostream>
#include <string>
using namespace std;

class IPSocketException{
private:
	string status;
public:
	IPSocketException();
	IPSocketException(string str);
	void dump(){
		cout << status << endl;
	}
	void setStatus(string status){
		this->status=status;
	}
	string getStatus(){
		return status;
	}
};

#endif /*IPSOCKETEXCEPTION_HH_*/
