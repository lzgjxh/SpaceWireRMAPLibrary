#include "IPSocketException.hh"

using namespace std;

IPSocketException::IPSocketException(){}

IPSocketException::IPSocketException(int str){
	setStatus(str);
}
