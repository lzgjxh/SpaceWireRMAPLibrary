#include "SpaceWireRTUtilities.hh"
using namespace std;

static unsigned int SpaceWireRTUtilities::calcCRC16(unsigned char *data, unsigned int length){
	unsigned int crc=0xFFFF;
	while (length--) {
		crc=((crc&0x00FF)<<8)^crc_table[((crc>>8)^*data++)&0x00FF];
	}
	return crc;
}
