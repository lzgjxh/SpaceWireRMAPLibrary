#include "IPSocketException.hh"

using namespace std;

IPSocketException::IPSocketException(){}

IPSocketException::IPSocketException(string str){
	setStatus(str);
}
