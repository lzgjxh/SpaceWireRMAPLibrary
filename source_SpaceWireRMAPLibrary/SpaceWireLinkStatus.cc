#include "SpaceWireLinkStatus.hh"
using namespace std;

const double SpaceWireLinkStatus::DefaultLinkSpeed=10;

SpaceWireLinkStatus::SpaceWireLinkStatus(){
	linkSpeed=SpaceWireLinkStatus::DefaultLinkSpeed;
	linkEstablished=false;
}

void SpaceWireLinkStatus::setLinkEstablishment(bool mode){
	linkEstablished=mode;
}

bool SpaceWireLinkStatus::isLinkEstablished(){
	return linkEstablished;
}

void SpaceWireLinkStatus::setLinkSpeed(double speedInMHz){
	linkSpeed=speedInMHz;
}

double SpaceWireLinkStatus::getLinkSpeed(){
	return linkSpeed;
}

void SpaceWireLinkStatus::setTimeout(double timeoutInMilliSec){
	timeout=timeoutInMilliSec;
}

double SpaceWireLinkStatus::getTimeout(){
	return timeout;
}

void SpaceWireLinkStatus::setComment(string newcomment){
	comment=newcomment;
}

string SpaceWireLinkStatus::getComment(){
	return comment;
}

void SpaceWireLinkStatus::dump(){
	cout << "SpaceWireLinkStatus" << endl;
	cout << " Link Established   : " << (linkEstablished? "Yes":"No") << endl;
	cout << " Link Speed         : " << linkSpeed << "MHz" << endl;
	cout << " Time Out           : " << timeout << "ms" << endl;
	cout << endl;
}

/** History
 * 2008-05-xx file created (Takayuki Yuasa)
 * 2008-10-08 added dump() method (Takayuki Yuasa)
 */
