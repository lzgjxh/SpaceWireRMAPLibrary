#include "SpaceWireRTException.hh"
using namespace std;

SpaceWireRTException::SpaceWireRTException(){
	setStatus(SpaceWireRTException::NoException);
}

SpaceWireRTException::SpaceWireRTException(int newstatus){
	setStatus(newstatus);
}

void SpaceWireRTException::setStatus(int newstatus){
	status=newstatus;
}

int SpaceWireRTException::getStatus(){
	return status;
}

void SpaceWireRTException::dump(){
	string str;
	cout << "SpaceWireRTException : ";
	switch(status){
		case NoException:
			str="No Exception";
			break;
	}
	cout << str << endl;
}