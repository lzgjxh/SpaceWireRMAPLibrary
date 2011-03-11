#include "RMAPUtilities.hh"
#include "RMAPDestination.hh"
using namespace std;

unsigned char RMAPUtilities::calcCRC(RMAPDestination& destination, vector<unsigned char>& data){
	unsigned char crc=0x00;
	unsigned char* RMAP_CRCTable;
	int length=data.size();
	if (destination.getCRCType()==RMAPDestination::RMAPCRCTypeDraftE){
		RMAP_CRCTable=(unsigned char*)RMAP_CRCTable_DraftE;
	} else if (destination.getCRCType()==RMAPDestination::RMAPCRCTypeDraftF){
		RMAP_CRCTable=(unsigned char*)RMAP_CRCTable_DraftF;
	}
	for (int i=0;i<length;i++){
		crc=RMAP_CRCTable[(crc^(data)[i]) & 0xff];
	}
	return crc;
}
