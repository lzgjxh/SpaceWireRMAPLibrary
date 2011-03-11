#include "SpaceWireException.hh"
using namespace std;

SpaceWireException::SpaceWireException(){
	setStatus(SpaceWireException::NoException);
}

SpaceWireException::SpaceWireException(int newstatus){
	setStatus(newstatus);
}

void SpaceWireException::setStatus(int newstatus){
	status=newstatus;
}

int SpaceWireException::getStatus(){
	return status;
}

void SpaceWireException::dump(){
	string str;
	cout << "SpaceWireException : ";
	switch(status){
		case OpenFailed:
			str="OpenFailed";
			break;
		case NoException:
			str="NoException";
			break;
		case NotImplemented:
			str="NotImplemented";
			break;
		case TransferNotStarted:
			str="TransferNotStarted";
			break;
		case TransferStarted:
			str="TransferStarted";
			break;
		case TransferSuccess:
			str="TransferSuccess";
			break;
		case TransferErrorDisconnect:
			str="TransferErrorDisconnect";
			break;
		case TransferErrorUnkown:
			str="TransferErrorUnkown";
			break;
		case TransferErrorTimeout:
			str="TransferErrorTimeout";
			break;
		case TransferErrorMemoryOverflow:
			str="TransferErrorMemoryOverflow";
			break;
		case SendFailed:
			str="SendFailed";
			break;
		case ReceiveFailed:
			str="ReceiveFailed";
			break;
		case SizeIncorrect:
			str="SizeIncorrect";
			break;
		case Disconnected:
			str="Disconnected";
			break;
		case DeviceDriverFailed:
			str="DeviceDriverFailed";
			break;
		case NoDevice:
			str="NoDevice";
			break;
		case SendTimeCodeError:
			str="SendTimeCodeError";
			break;
		case GetTimeCodeError:
			str="GetTimeCodeError";
			break;
	}
	cout << str << endl;
}

/** History
 * 2008-05-xx file created (Takayuki Yuasa)
 * 2008-10-08 added dump() implementation (Takayuki Yuasa)
 * 2008-12-17 added SendTimeCodeError and GetTimeCodeError (Takayuki Yuasa)
 */
