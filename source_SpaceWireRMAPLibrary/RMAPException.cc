#include "RMAPException.hh"
using namespace std;

RMAPException::RMAPException(){
	status=0;
}

RMAPException::RMAPException(int newstatus){
	status=newstatus;
}

void RMAPException::setStatus(int newstatus){
	status=newstatus;
}

int RMAPException::getStatus(){
	return status;
}

void RMAPException::dump(){
	cout << "RMAPException : ";
	string str;
	switch(status){
		case NotAnRMAPPacket:
			str="NotAnRMAPPacket";
			break;
		case InvalidHeaderCRC:
			str="InvalidHeaderCRC";
			break;
		case InvalidDataCRC:
			str="InvalidDataCRC";
			break;
		case InvalidDestination:
			str="InvalidDestination";
			break;
		case TooManySockets:
			str="TooManySockets";
			break;
		case SocketIDOutOfRange:
			str="SocketIDOutOfRange";
			break;
		case TooManyRequests:
			str="TooManyRequests";
			break;
		case TIDOutOfRange:
			str="TIDOutOfRange";
			break;
		case RMAPUnsuccessfull:
			str="RMAPUnsuccessfull";
			break;
		case RMAPEngine_MultipleTransactionScheme_Suspended:
			str="RMAPEngine_MultipleTransactionScheme_Suspended";
			break;
		case RMAPEngine_MultipleTransactionScheme_NotSuspended:
			str="RMAPEngine_MultipleTransactionScheme_NotSuspended";
			break;
		case ByteOrderError:
			str="ByteOrderError";
			break;
		case BadWordWidth:
			str="BadWordWidth";
			break;
		case TimeoutNotEnabled:
			str="TimeoutNotEnabled";
			break;
		case Timedout:
			str="TimedOut";
			break;
	}
	cout << str << endl;
}
