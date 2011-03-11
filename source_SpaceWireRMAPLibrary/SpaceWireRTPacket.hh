#ifndef SPACEWIRERTPACKET_HH_
#define SPACEWIRERTPACKET_HH_

#include "SpaceWirePacket.hh"
#include "SpaceWireRTUtilities.hh"

#include <vector>
#include <iostream>
using namespace std;

/** SpaceWireRTPacket class handles creation and
 * interpretation of packets used in a SpaceWire-RT
 * network. To create a packet, users should make
 * an instance of the class, and then set properties
 * of the intended packet. createPacket() method
 * compiles the properties, calculates CRC (Cyclic
 * Redundancy Check), and creates the packet
 * byte array. The generated packet array is retained 
 * in the private vector class of the instance.
 * A getter method for the packet content, getPacket(),
 * returns the pointer of the vector. 
 *  
 * Packet interpretation can be performed by setting
 * byte array to an instance of SpaceWireRTPacket
 * class via setPacket() method, and by calling
 * interpretPacket() method. interpretPacket() reads
 * the byte array, and extracts information contained
 * in the packet. The information of the packet can
 * be accessed via getter methods for individual
 * properties (e.g. getSequenceNumber()).
 * 
 * Packet Structures
 * 
 * =================================
 * PDU Encapsulation
 * =================================
 * 0 [Destination Path Address]
 * 0 [Destination Path Address]
 *   ...
 * 0 [Destination Path Address]
 *   --------------------------
 * 0 [Destination Logical Address]
 * 1 [SpaceWire-RT Protocol ID = 0x03]
 * 2 [Source Logical Address]
 * 3 [Channel]
 * 4 [Type/Redundancy]
 * 5 [Sequence Number]
 * 6 [Data Length]
 * 7 [Header CRC]
 *   --------------------------
 * 8 [Data]
 * 9 [Data]
 *   ...
 * X [CRC Most-Significant (MS) 8 bits]
 *X+1[CRC Lowest-Significant (LS) 8 bits]
 *   [EOP]
 * 
 * =================================
 * Control Code Encapsulation
 * =================================
 * (# of byte) [Content of the Byte]
 * 0 [Destination Path Address]
 * 0 [Destination Path Address]
 *   ...
 * 0 [Destination Path Address]
 *   --------------------------
 * 0 [Destination Logical Address]
 * 1 [SpaceWire-RT Protocol ID = 0x03]
 * 2 [Source Logical Address]
 * 3 [Channel]
 * 4 [Type = ACK, BFCT, or BACK]
 * 5 [Sequence Number]
 * 6 [Reserved = 0]
 * 7 [Header CRC]
 *   [EOP]
 */
class SpaceWireRTPacket : public SpaceWirePacket {
private:
	SpaceWireRTChannel* spacewirertchannel;
	vector<unsigned char> header;
	vector<unsigned char> data;
private:
	unsigned int headerCRC;
	unsigned int dataCRC;
	unsigned char eopType;
public:
	static const unsigned char SpaceWireRTProtocolID=0x03;
private:
	unsigned char type;
	unsigned char sequencenumber;
	const unsigned char reservedbyte=0x00;
public:
	SpaceWireRTPacket(){
		header.clear();
		data.clear();
		headerCRC=0;
		dataCRC=0;
	}
	~SpaceWireRTPacket(){}
public:
	void createPacket();
	vector<unsigned char>* getPacket();
	void setPacket(vector<unsigned char>* newpacket) throw(SpaceWireRTException);
	void interpretPacket() throw(SpaceWireRTException);
	vector<unsigned char>* getHeader(){
		return &header;
	}
	vector<unsigned char>* getData(){
		return &data;
	}
	void setData(vector<unsigned char>* newdata){
		data=*newdata; //copy byte-to-byte
	}
	void setSpaceWireRTDestination(SpaceWireRTDestination newdestination){
		SpaceWirePacket::setDestination((SpaceWireDestination)newdestinaion);
		spacewirertdestination=newdestination;//copy byte-to-byte
	}
	void setDestination(SpaceWireRTDestination newdestination){
		setSpaceWireRTDestination(newdestination);
	}
	SpaceWireRTDestination* getSpaceWireRTDestination(){
		return &spacewirertdestination;
	}
	unsigned int getHeaderCRC(){
		headerCRC=SpaceWireRTUtilities::calcCRC(spacewirertdestination,header);
		return headerCRC;
	}
	unsigned int getDataCRC(){
		dataCRC=SpaceWireRTUtilities::calcCRC(spacewirertdestination,data);
		return dataCRC;
	}
	void dump();
	void dumpPacket();
public:
};


#endif /*SPACEWIRERTPACKET_HH_*/

/** History
 * 2008-08-26 file created (Takayuki Yuasa)
 */