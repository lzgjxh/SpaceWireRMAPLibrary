#ifndef SPACEWIRERTCOMMUNICATIONMODEL_HH_
#define SPACEWIRERTCOMMUNICATIONMODEL_HH_

#include "SpaceWireRTException.hh"

using namespace std;

class SpaceWireRTCommunicationModel {
private:
	int communicationmodeltype;
public:
	static const int AsynchronousModel=0x00;
	static const int SynchronousModel=0x02;
	static const int DefaultCommunicationModel=AsynchronousModel;
public:
	SpaceWireRTCommunicationModel();
	SpaceWireRTCommunicationModel(int newcommunicationmodeltype)  throw(SpaceWireRTException);
	~SpaceWireRTCommunicationModel();
public:
	int getCommunicationModel();
	void setCommunicationModel(int newcommunicationmodeltype) throw(SpaceWireRTException);
public:
	bool isAsynchronousModel(){
		if(communicationmodeltype==AsynchronousModel){
			return true;
		}else{
			return false;
		}
	}
	bool isSynchronousModel(){
		if(communicationmodeltype==SynchronousModel){
			return true;
		}else{
			return false;
		}
	}
private:
	bool isValidCommunicationModelType(int newcommunicationmodeltype);
};

#endif /*SPACEWIRERTCOMMUNICATIONMODEL_HH_*/

/** History
 * 2008-08-26 file created (Takayuki Yuasa)
 */