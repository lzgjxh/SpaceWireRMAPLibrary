#ifndef RMAPDESTINATION_HH_
#define RMAPDESTINATION_HH_

#include "RMAPException.hh"
#include "SpaceWireCLI.hh"
#include "SpaceWireDestination.hh"
#include "SpaceWireUtilities.hh"

#include <iostream>
#include <iomanip>
#include <vector>
using namespace std;

/** A class which holds properties of destination 
 * and source RMAP nodes such as Logical/Path address,
 * destination key, etc.
 */ 
class RMAPDestination : public SpaceWireDestination {
private:

	/** RMAP Destination Key (8bits).
	 */
	unsigned char destinationKey;
	
	/** RMAP CRC type.
	 * crcType=0 is Draft E CRC.
	 * crcType=1 is Draft F CRC.
	 */
	unsigned char crcType;
	
	/** Source Logical Address. 
	 */
	unsigned char sourceLogicalAddress;
	
	/** Source Path Address.
	 * A path address which is needed to transport an
	 * RMAP packet in Path Addressing mode.
	 */
	vector<unsigned char> sourcePathAddress;
	
	/** Variable for Source Path Address Length (SPAL).
	 * In RMAP Command Packet, SPAL exactly corresponds to
	 * the length of Source Path Address. Otherwise, in
	 * RMAP Reply Packet, they can be different from each other.
	 * In other words, Source Path Address can be removed
	 * during it passes Routers, whereas SPAL remains as
	 * its original value.
	 */
	unsigned char sourcePathAddressLength;
	
	/** Width of "1 Word" in the destination counted in
	 * a unit of byte. This word width information is
	 * used to re-aligning data array so as to follow
	 * the definition of the RMAP standard (big-endian)
	 * in RMAPPacket class.
	 * For example, NEC SpaceWireRouter Board treats
	 * 4 bytes as 1 word. 
	 */
	unsigned int wordwidth;
	
public:
	/** Constructor for RMAPDestination. In the
	 * constructor, individual parameters are
	 * set to default by calling setDefaultDestination().
	 */
	RMAPDestination();
	
	/** Copy Constructor.
	 */
	RMAPDestination(const RMAPDestination &original){
		clone((RMAPDestination*)&original);
	}
	
	/** Substitution Operator.
	 */
	RMAPDestination &operator=(const RMAPDestination &original){
		clone((RMAPDestination*)&original);
		return *this;
	}
	
	/** Create clone of this class.
	 */
	void clone(RMAPDestination* original){
		setDefaultDestination();
		setDestinationKey(original->getDestinationKey());
		setCRCType(original->getCRCType());
		setDestinationLogicalAddress(original->getDestinationLogicalAddress());
		setDestinationPathAddress(original->getDestinationPathAddress());
		setSourceLogicalAddress(original->getSourceLogicalAddress());
		setSourcePathAddress(original->getSourcePathAddress());
		setWordWidth(original->getWordWidth());
	}
	
	/** Set default destination.
	 */
	void setDefaultDestination();
	
	/** Setter for unsigned char destinationKey.
	 * @param key destination key expressed in unsigned char
	 */
	inline void setDestinationKey(unsigned char key){
		destinationKey=key;
	}
	
	/** Getter for unsigned char destinationKey.
	 * @return destination key expressed in unsigned char
	 */
	inline unsigned char getDestinationKey(){
		return destinationKey;
	}
	
	/** Setter for CRC type (1=Draft F,0=Draft E).
	 * @param mode CTC Type expressed in unsigned char
	 */
	inline void setCRCType(unsigned char mode){
		crcType=(mode&0x01);
	}
	
	/** Setter for CRC type (Draft E).
	 */
	inline void setDraftECRC(){
		crcType=0;
	}
	
	/** Setter for CRC type (Draft F).
	 */
	inline void setDraftFCRC(){
		crcType=1;
	}
	
	/** Getter for CRC type.
	 * @return CTC Type expressed in unsigned char
	 */
	inline unsigned char getCRCType(){
		return crcType;
	}
	
	/** Setter for sourceLogicalAddress.
	 */
	inline void setSourceLogicalAddress(unsigned char address){
		sourceLogicalAddress=address;
	}
	
	/** Getter for sourceLogicalAddress.
	 */
	inline unsigned char getSourceLogicalAddress(){
		return sourceLogicalAddress;
	}

	/** Setter for sourcePathAddress.
	 * @return a vector which contains source path address
	 */
	inline void setSourcePathAddress(vector<unsigned char> address){
		sourcePathAddress=address;
	}
	
	/** Setter for sourcePathAddress.
	 */
	inline void clearSourcePathAddress(){
		sourcePathAddress.clear();
	}
	
	/** Getter for sourcePathAddress.
	 * @param a vector which contains source path address
	 */
	inline vector<unsigned char> getSourcePathAddress(){
		return sourcePathAddress;
	}

	/** Setter for sourcePathAddressLength.
	 * @param newspal new source path address length
	 */
	inline void setSourcePathAddressLength(unsigned char newspal){
		sourcePathAddressLength=newspal;
	}
	
	/** Setter for sourcePathAddressLength.
	 * The value is automatically calculated from
	 * configured source path address vector size.
	 */
	inline void setSourcePathAddressLength(){
		sourcePathAddressLength=(unsigned char)((sourcePathAddress.size()+3)/4);
	}

	/** Getter for sourceLogicalAddressLength.
	 * @return source path address length
	 */
	inline unsigned char getSourcePathAddressLength(){
		return sourcePathAddressLength;
	}
	
	/** Compare RMAPDestination.
	 */
	inline bool equals(RMAPDestination* objectrmapdestination){
		//現状、Source Logical Addressが等しいかだけで、二つのRMAPDestinationの比較を
		//している。なにかもう少しよい判定条件がないものか。
		if(
				//destinationKey==objectrmapdestination->getDestinationKey() &&　//Replyパケットからは復元不能なのでコメントアウト
				//crcType==objectrmapdestination->getCRCType() && //Commandパケット、Replyパケット両方ともから復元不能なのでコメントアウト
				getSourceLogicalAddress()==objectrmapdestination->getSourceLogicalAddress() //&&
				//getDestinationLogicalAddress()==objectrmapdestination->getDestinationLogicalAddress() &&
				//Destination側のLogical Addressは、ルータなどの場合、レジスタを書き換えることで変化するし、
				//しかもその変更の祭のRMAPアクセスでは、CommandとReplyでLogical Addressが変わることになるので、
				//上記の判定文も必ずしも満たされないので、コメントアウトしてある
				//getDestinationPathAddress()==objectrmapdestination->getDestinationPathAddress()
				//これもReplyパケットからは復元不能なのでコメントアウト
		){
			return true;
		}else{
			return false;
		}
	}
	
	/** Setter for wordwidth.
	 * @param newwordwidth word width in a unit of byte
	 */
	inline void setWordWidth(unsigned int newwordwidth) throw (RMAPException){
		if(newwordwidth!=0){
			wordwidth=newwordwidth;
		}else{
			throw(RMAPException(RMAPException::BadWordWidth));
		}
	}
	
	/** Getter for wordwidth.
	 * @return word width in a unit of byte
	 */
	inline unsigned int getWordWidth(){
		return wordwidth;
	}
	
	/** Show command line interface to set RMAPDestination
	 * properties.
	 */
	void setRMAPDestinationViaCommandLine();

	/** Dump this RMAPDestination information.
	 */
	void dump();

public:
	static const unsigned char DefaultLogicalAddress=0xFE;
	static const unsigned char DefaultDestinationKey=0x00;
	static const unsigned char RMAPCRCTypeDraftE=0x00;
	static const unsigned char RMAPCRCTypeDraftF=0x01;
	static const unsigned char DefaultCRCType=RMAPCRCTypeDraftF;
	static const unsigned int DefaultWordWidth=1;
};


#endif /*RMAPDESTINATION_HH_*/

/* History
 * 20080818 Takayuki Yuasa set default CRC version to F from E.
 */
