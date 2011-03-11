#include "SpaceWireRTCommunicationModel.hh"
using namespace std;

SpaceWireRTCommunicationModel::SpaceWireRTCommunicationModel(){
	communicationmodeltype=DefaultCommunicationModel;
}

SpaceWireRTCommunicationModel::SpaceWireRTCommunicationModel(int newcommunicationmodeltype) throw(SpaceWireRTException){
	setCommunicationModel(newcommunicationmodeltype);
}

SpaceWireRTCommunicationModel::~SpaceWireRTCommunicationModel(){}

int SpaceWireRTCommunicationModel::getCommunicationModel(){
	return communicationmodeltype;
}

void SpaceWireRTCommunicationModel::setCommunicationModel(int newcommunicationmodeltype)  throw(SpaceWireRTException){
	if(isValidCommunicationModelType(newcommunicationmodeltype)){
		communicationmodeltype=newcommunicationmodeltype;
	}else{
		throw SpaceWireRTException(SpaceWireRTException::InvalidCommunicationModel);
	}
}

bool SpaceWireRTCommunicationModel::isValidCommunicationModelType(){
	if(communicationmodeltype==AsynchronousModel ||
			communicationmodeltype==SynchronousModel){
		return true;
	}else{
		return false;
	}
}

/** History
 * 2008-08-26 file created (Takayuki Yuasa)
 */