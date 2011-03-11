#ifndef RMAPPACKET_HH_
#define RMAPPACKET_HH_

#include "RMAPDestination.hh"
#include "RMAPException.hh"
#include "RMAPUtilities.hh"
#include "SpaceWireDestination.hh"
#include "SpaceWirePacket.hh"
#include "SpaceWireUtilities.hh"

#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

/** RMAPPacket class handles creation and interpretation of
 * RMAP packets. To create an RMAP packet, users have to set
 * properties of the packet, then call createPacket().
 * To interpret an RMAP packet (e.g. after receiving reply
 * packet), first call setPacket() then call get***()
 * to get properties of the packet.
 * Since this class also handles the "byte order problem" which
 * includes re-alignment of data when an RMAP destination
 * deines "1 Word" as multiple bytes (e.g. 4bytes-1word),
 * users are not bothered by byte order (or word width) of
 * RMAP destinations. Users can pass data vector which is aligned
 * with "1byte-1word" style to RMAPPacket class. RMAPPacket class
 * automatically re-aligns the data vector so as to follow
 * a word width property of designated RMAPDestination and
 * the definition of the RMAP standard (big-endian).
 */  
class RMAPPacket : public SpaceWirePacket {
private:
	RMAPDestination destination;
	
	vector<unsigned char> header;
	vector<unsigned char> data;
	vector<unsigned char> data1byte1word;
	
	/** Inherited from SpaceWirePacket class
	 * 	vector<unsigned char> packet;
	 *  SpaceWireDestination destination;
	 */
	
private:
	vector<unsigned char> tmppathaddress;
	
public:
	/** Default constructor. Private variables are intialized
	 * in this constructor.
	 */
	RMAPPacket(){
		header.clear();
		data.clear();
		headerCRC=0;
		dataCRC=0;
		
		crcCheckMode=RMAPDefaultCRCCheckMode;
		
		commandOrReply=RMAPPacket::RMAPDefaultPacketType;
		writeOrRead=RMAPPacket::RMAPDefaultWriteOrRead;
		verify=RMAPPacket::RMAPDefaultVerifyMode;
		ack=RMAPPacket::RMAPDefaultAckMode;
		increment=RMAPPacket::RMAPDefaultIncrementMode;
		transactionID=RMAPPacket::RMAPDefaultTID;
		extendedAddress=RMAPPacket::RMAPDefaultExtendedAddress;
		address=RMAPPacket::RMAPDefaultAddress;
		length=RMAPPacket::RMAPDefaultLength;
		headerCRC=RMAPPacket::RMAPDefaultHeaderCRC;
		dataCRC=RMAPPacket::RMAPDefaultDataCRC;
		replyStatus=RMAPPacket::RMAPDefaultReplyStatus;
	}

	/** Copy Constructor.
	 */
	RMAPPacket(const RMAPPacket &original){
		clone((RMAPPacket*)&original);
	}
	
	/** Substitution Operator.
	 */
	RMAPPacket &operator=(const RMAPPacket &original){
		clone((RMAPPacket*)&original);
		return *this;
	}
	
	/** Clone this instance.
	 */
	void clone(RMAPPacket* original){
		header=*(original->getHeader());
		data=*(original->getData());
		headerCRC=original->getHeaderCRC();
		dataCRC=original->getDataCRC();
		
		crcCheckMode=original->getCRCCheckMode();
		
		commandOrReply=original->getPacketType();
		writeOrRead=original->getWriteOrRead();
		verify=original->getVerifyMode();
		ack=original->getAckMode();
		increment=original->getIncrementMode();
		transactionID=original->getTransactionID();
		extendedAddress=original->getExtendedAddress();
		address=original->getAddress();
		length=original->getLength();
		replyStatus=original->getReplyStatus();
	}
	
	/** Destructor.
	 */
	~RMAPPacket(){}
	
	/** Creates an RMAP packet based on the configured properties.
	 * This method stores the created packet to "packet" vector
	 * (private field member of SpaceWirePacket class). The created
	 * packet containt can be obtained by calling getPacket().
	 * In other words, users should call createPacket() before
	 * invoking getPacket().
	 */
	void createPacket();
		
	/** Gets created RMAP packet. 
	 * @retrun a reference to a vector which holds the current packet
	 */
	vector<unsigned char>* getPacket();
	
	/** Sets packet containt to interpret RMAP Reply packet.
	 * @param newpacket a reference to a vector which holds new packet containt
	 */
	void setPacket(vector<unsigned char>* newpacket) throw(RMAPException);

	/** Sets new packet buffer pointer.
	 * @param newpacketbuffer a reference to a vector instance reserved on the heap space
	 */
	void setPacketBuffer(vector<unsigned char>* newpacketbuffer) throw(RMAPException);
	
	/** Returns packet buffer pointer.
	 * @return a reference to packet buffer
	 */
	vector<unsigned char>* getPacketBuffer() throw(RMAPException);
	
	/** Interprets an RMAP packet, and extracts properties (destination,
	 * destination key, address, data length, crcs) and data.
	 * 
	 * Example:How to retrieve Data part from a reply packet.
	 *  vector<unsigned char> v=spacewireif.receivePacket();
	 *  rmappacket.setPacket(v);
	 *  rmappacket.interpretPacket();
	 *  vector<unsigned data> data=rmappacket.getDta();
	 */
	void interpretPacket() throw(RMAPException);
	
	
	/** Returns Header part of this RMAP packet.
	 * Before the invokation of this method, createPacket() or
	 * setPacket() methods should be called to fill the Header
	 * part, otherwise vector class with size 0 will be returned.
	 * Header CRC is not included in the returned vector.
	 * @return a reference to a vector which holds header part of this packet
	 */
	inline vector<unsigned char>* getHeader(){
		return &header;
	}
	
	/** Returns Data part of this RMAP packet.
	 * Before the invokation of this method, createPacket() or
	 * setPacket() methods should be called to fill the Data
	 * part, otherwise vector class with size 0 will be returned.
	 * Data CRC is not included in the returned vector.
	 * @return a reference to a vector which holds data part of this packet
	 */
	inline vector<unsigned char>* getData() throw (RMAPException){
		//re-aligning byte order
		unsigned int wordwidth=destination.getWordWidth();
		if(wordwidth==0x01){
			//no re-alignment is needed
			return &data;
		}else{
			//re-align
			if(data.size()%wordwidth==0){
				data1byte1word.clear();
				for(unsigned int i=0;i<data.size()/wordwidth;i++){
					for(unsigned int o=0;o<wordwidth;o++){
						data1byte1word.push_back(data.at((i+1)*wordwidth-(o+1)));
					}
				}
				return &data1byte1word;
			}else{
				//data size is not a multiple of word width
				throw(RMAPException(RMAPException::ByteOrderError));
			}
		}
	}
	
	/** Setter for Data part. The data should be ordered in
	 * 1byte-1word style. In this method, RMAPPacket automatically
	 * re-align the order according to the destination property
	 * which is obtained via RMAPDestination::getWordWidth().
	 * @param newdata a reference to a data vector
	 */
	inline void setData(vector<unsigned char>* newdata) throw (RMAPException){
		//re-aligning byte order
		unsigned int wordwidth=destination.getWordWidth();
		if(wordwidth==0x01){
			//no re-alignment is needed
			data=*newdata; //copy byte-to-byte
		}else{
			//re-align
			if(newdata->size()%wordwidth==0){
				data.clear();
				for(unsigned int i=0;i<newdata->size()/wordwidth;i++){
					for(unsigned int o=0;o<wordwidth;o++){
						data.push_back(newdata->at((i+1)*wordwidth-(o+1)));
					}
				}
			}else{
				//data size is not a multiple of word width
				throw(RMAPException(RMAPException::ByteOrderError));
			}
		}
		length=data.size();
	}
	
	/** Setter for Reply status.
	 * Usually this method is used to create Reply packet or
	 * in interpretPacket() method.
	 * @param status an unsinged char value which hold one of
	 * RMAPReplyStatus enum
	 */
	inline void setReplyStatus(unsigned char status){
		replyStatus=status;
	}
	/** Get status value of an RMAP reply packet.
	 * @return status of reply packet (enum RMAPReplyStatus)
	 */
	inline unsigned char getReplyStatus(){
		return replyStatus;
	}
	
	/** Sets the destination of this RMAP packet.
	 * Since RMAPPacket class inherits SpaceWirePacket class,
	 * SpaceWirePacket.setDestination(SpaceWireDestination) method
	 * is also called in this method besides setting private
	 * variables of RMAPPacket class.
	 * @param newdestination destination of this RMAP access expressed in RMAPDestination class
	 */ 
	inline void setRMAPDestination(RMAPDestination newdestination){
		SpaceWirePacket::setDestination((SpaceWireDestination)newdestination);
		destination=newdestination;
#ifdef DEBUG
		cout << "RMAPPacket::setRMAPDestination()" <<endl;
		cout << "Destination Path Address" << endl;
		SpaceWireUtilities::dumpPacket(&destination.getDestinationPathAddress());
		cout << "Source Path Address" << endl;
		SpaceWireUtilities::dumpPacket(&destination.getSourcePathAddress());
#endif
	}
	
	/** Setter for RMAPDestination. This method overrides
	 * SpaceWireDestination::setDestination() to eliminate
	 * ambiguity in method name between setRMAPDestination().
	 */
	inline void setDestination(RMAPDestination newdestination){
		setRMAPDestination(newdestination);
	}

	
	/** Gets the RMAPDestination of this RMAP packet.
	 * If users want to get the destination as
	 * SpaceWireDestination class, there are two alternative
	 * ways to do so; (1) to call getDestination() method of
	 * super class, (2) to cast the RMAPDestination class
	 * returned from getRMAPDestination() to SpaceWireDestination
	 * class. 
	 * @return RMAPDestination class which is currently set to this RMAPPacket class
	 */
	inline RMAPDestination* getRMAPDestination(){
		return &destination;
	}
	
	/** Calculates Header CRC.
	 * When calculates the header CRC, Destination Path Address must
	 * be excluded from the CRC calc scope. The Destination Path Address
	 * is a SpaceWire-layer data, and the header CRC is just for the
	 * header part of an RMAP-layer packet.<br>
	 * [Destination Path Address][RMAP Header+Header CRC][RMAP Data+Data CRC]
	 * @return calculated CRC for header section
	 */
	inline unsigned char getHeaderCRC(){
		headerCRC=RMAPUtilities::calcCRC(destination,header);
		return headerCRC;
	}
	
	/** Calculates Data CRC.
	 * @return calculated CRC for data section
	 */
	inline unsigned char getDataCRC(){
		dataCRC=RMAPUtilities::calcCRC(destination,data);
		return dataCRC;
	}
	
	/** Dump packet content for debugging.
	 */
	void dump();
	
	/** Dump packet content for debugging.
	 */
	void dumpPacket();
	
public:
	bool crcCheckMode;
	void enableCRCCheck(){
		crcCheckMode=true;
	}
	void disableCRCCheck(){
		crcCheckMode=false;
	}
	bool getCRCCheckMode(){
		return crcCheckMode;
	}
private:
	/** Creates an RMAP packet header. This method is invoked inside
	 * createPacket() method. The resulting header part is stored to
	 * header vector.
	 */
	void createHeader();
	void dumpCommandPacket();
	void dumpReplyPacket();
	
private:
	/* Packet Type, Command/Reply, Source Path Address Length
	 */
	unsigned char pt_c_spal;
	
	/** RMAP Command/Reply (1bit).
	 * commandOrReply=RMAPPacketTypeCommand is RMAP Command.
	 * commandOrReply=RMAPPacketTypeReply is RMAP Reply.
	 * The least significant bit (LSB) of unsigned char
	 * type will be used.
	 */
	unsigned char commandOrReply;
	
	/** RMAP Write/Read (1bit).
	 * writeOrRead=RMAPPacketWriteMode is RMAP Write.
	 * writeOrRead=RMAPPacketReadMode is RMAP Read.
	 * The least significant bit (LSB) of unsigned char
	 * type will be used.
	 */
	unsigned char writeOrRead;
	
	/** RMAP Verify Mode (1bit).
	 * verify=RMAPPacketVerifyMode is Verify mode.
	 * verify=RMAPPacketNonVerifyMode is Non Verify mode.
	 * The least significant bit (LSB) of unsigned char
	 * type will be used.
	 */
	unsigned char verify;

	/** RMAP Acknowledge Mode (1bit).
	 * ack=RMAPPacketAckMode is Ack mode.
	 * ack=RMAPPacketNoAckMode is No Ack mode.
	 * The least significant bit (LSB) of unsigned char
	 * type will be used.
	 */
	unsigned char ack;
	
	/** RMAP Increment Mode (1bit).
	 * increment=RMAPPacketIncrementMode is Increment mode.
	 * increment=RMAPPacketNoIncrementMode is No Increment mode.
	 * The least significant bit (LSB) of unsigned char
	 * type will be used.
	 */
	unsigned char increment;
	
	/** RMAP TransactionID (16bits).
	 * Value stored in lower 16bits of unsigned int type will be used. 
	 */
	unsigned int transactionID;
	
	/** RMAP Extended Address (8bits).
	 */
	unsigned char extendedAddress;
	
	/** RMAP Address (32bits).
	 * Address of memory or fifo, etc, which is accessed in
	 * the RMAP operation. Value stored in lower 32bits of
	 * unsigned int type will be used.
	 */
	unsigned int address;
	
	/** RMAP Data Length (24bits).
	 * Length of data which are read/written from/to in
	 * the RMAP operation. Value stored in lower 24bits of
	 * unsigned int type will be used.
	 */
	unsigned int length;
	
	/** RMAP Header CRC (8bits).
	 */
	unsigned char headerCRC;
	
	/** RMAP Data CRC (8bits).
	 */
	unsigned char dataCRC;
	
	/** End of Packet type of sent/received packet.
	 */
	unsigned char eopType;
	
	/** Reply Status.
	 */
	unsigned char replyStatus;
	
public:
	// Accessors
	/*
	 * unsigned char commandOrReply;
	 * unsigned char writeOrRead;
	 * unsigned char verify;
	 * unsinged char ack;
	 * unsigned char increment;
	 * unsigned int transactionID
	 * unsigned char extendedAddress
	 * unsigned int address;
	 * unsigned int length;
	 * unsigned char headerCRC;
	 * unsigned char dataCRC;
	 * unsigned char eopType;
	 * unsigned char replyStatus;
	 */
public:
	/** Getter for (Packet Type, Command, SPAL) part of this packet.
	 * @return an unsigned char value which contains 8-bit information
	 * on Packet Type, Command, Veirfy mode, Ack mode, Increment mode,
	 * Source Path Address Length
	 */
	inline unsigned char getPacketTypeCommandSPAL_for_packetCreation(){
		unsigned char spal_calculated_from_sourcepathaddress=((destination.getSourcePathAddress().size()+3)/4);
		unsigned char spal_currently_configurated=destination.getSourcePathAddressLength();
		unsigned char spal_returned;
		if(spal_calculated_from_sourcepathaddress==spal_currently_configurated){
			spal_returned=spal_calculated_from_sourcepathaddress;
		}else{
			spal_returned=spal_calculated_from_sourcepathaddress;
		}
		return (unsigned char)(0*0x80+commandOrReply*0x40+writeOrRead*0x20+
				verify*0x10+ack*0x08+increment*0x04+spal_returned*0x01);
	}
	inline unsigned char getPacketTypeCommandSPAL_for_packetInterpretation(){
		return (unsigned char)(0*0x80+commandOrReply*0x40+writeOrRead*0x20+
				verify*0x10+ack*0x08+increment*0x04+getSourcePathAddressLength()*0x01);
	}
	
	/** Setter for Command/Reply packet type.
	 * @param mode an unsigned char value which holds RMAPPacketTypeCommand or RMAPPacketTypeReply
	 */
	inline void setPacketType(unsigned char mode){
		commandOrReply=(mode&0x01);
	}
	/** Setter for Command/Reply packet type.
	 */
	inline void setCommandPacket(){
		commandOrReply=RMAPPacketTypeCommand; //Command
	}
	/** Setter for Command/Reply packet type.
	 */
	inline void setReplyPacket(){
		commandOrReply=RMAPPacketTypeReply; //Reply
	}
	/** Getter for Command/Reply packet type.
	 * @return an unsigned char value which holds RMAPPacketTypeCommand or RMAPPacketTypeReply
	 */	
	inline unsigned char getPacketType(){
		return commandOrReply;
	}
	/** Getter for Command/Reply packet type.
	 * @return true if this packet is RMAP Command
	 */
	inline bool isCommandPacket(){
		if(commandOrReply==RMAPPacketTypeCommand){
			return true;
		}else{
			return false;
		}
	}
	/** Getter for Command/Reply packet type.
	 * @return true if this packet is RMAP Reply
	 */
	inline bool isReplyPacket(){
		if(commandOrReply==RMAPPacketTypeReply){
			return true;
		}else{
			return false;
		}
	}
	
	/** Setter for read/write mode.
	 * @param mode an unsigned value which holds RMAPPacketWriteMode or RMAPPacketReadMode
	 */
	inline void setWriteOrRead(unsigned char mode){
		writeOrRead=(mode&0x01); //The LSB is used.
		if(isReadMode()){
			/** Based on RMAP standard, the verify mode flag
			 * should be turn off(i.e. No Verify) in Read Command.
			 */
			setNoVerifyMode();
		}
	}
	/** Setter for read/write mode.
	 */
	inline void setWriteMode() {
		writeOrRead=RMAPPacketWriteMode; //Write
	}
	/** Setter for read/write mode.
	 */
	inline void setReadMode() {
		writeOrRead=RMAPPacketReadMode; //Read
		/** Based on RMAP standard, the verify mode flag
		 * should be turn off(i.e. No Verify) in Read Command.
		 */ 
		setNoVerifyMode();
	}
	/** Getter for read/write mode.
	 * @return RMAPPacketWriteMode or RMAPPacketReadMode
	 */
	inline unsigned char getWriteOrRead() {
		return writeOrRead;
	}
	/** Getter for read/write mode.
	 * @return true if this packet is Write packet
	 */
	inline bool isWriteMode(){
		if(writeOrRead==RMAPPacketWriteMode){
			return true;
		}else{
			return false;
		}
	}
	/** Getter for read/write mode.
	 * @return true if this packet is Read packet
	 */
	inline bool isReadMode() {
		if (writeOrRead==RMAPPacketReadMode) {
			return true;
		} else {
			return false;
		}
	}
	
	/** Setter for verify mode.
	 * @param mode an unsigned char value which holds RMAPPacketVerifyMode or RMAPPacketNonVerifyMode
	 */
	inline void setVerifyMode(unsigned char mode){
		verify=(mode&0x01); //The LSB is used.
	}
	/** Setter for verify mode.
	 */
	inline void setVerifyMode() {
		verify=RMAPPacketVerifyMode; //Verify
	}
	/** Setter for verify mode.
	 */
	inline void setNoVerifyMode() {
		verify=RMAPPacketNoVerifyMode; //No Verify
	}
	/** Getter for verify mode.
	 * @return verify an unsigned char value which holds RMAPPacketVerifyMode or RMAPPacketNonVerifyMode
	 */
	inline unsigned char getVerifyMode(){
		return verify;
	}
	/** Getter for verify mode.
	 * @return true if this packet is very mode
	 */
	inline bool isVerifyMode() {
		if (verify==RMAPPacketVerifyMode) {
			return true;
		} else {
			return false;
		}
	}
	/** Getter for verify mode.
	 * @return true if this packet is no-very mode
	 */
	inline bool isNonVerifyMode() {
		if (verify==RMAPPacketNoVerifyMode) {
			return true;
		} else {
			return false;
		}
	}
	
	/** Setter for ack mode.
	 * @param mode an unsigned char value which holds RMAPPacketAckMode or RMAPPacketNoAckMode
	 */
	inline void setAckMode(unsigned char mode){
		ack=(mode&0x01); //The LSB is used.
	}
	/** Setter for ack mode.
	 */
	inline void setAckMode() {
		ack=RMAPPacketAckMode; //Ack
	}
	/** Setter for ack mode.
	 */
	inline void setNoAckMode() {
		ack=RMAPPacketNoAckMode; //No Ack
	}
	/** Getter for ack mode.
	 * @return ack an unsigned char value which holds RMAPPacketAckMode or RMAPPacketNoAckMode
	 */  
	inline unsigned char getAckMode(){
		return ack;
	}
	/** Getter for ack mode.
	 * @return true if this packet is ack mode
	 */
	inline bool isAckMode() {
		if (ack==RMAPPacketAckMode) {
			return true;
		} else {
			return false;
		}
	}
	/** Getter for ack mode.
	 * @return true if this packet is no ack mode
	 */
	inline bool isNoAckMode() {
		if (ack==RMAPPacketNoAckMode) {
			return true;
		} else {
			return false;
		}
	}

	/** Setter for increment mode.
	 * If increment mode is set, the local address of the remote
	 * nodde is automatically incremented in multiple-byte
	 * read/write access. If not, the same address is repeatedly. 
	 * @param mode an unsigned char value which holds RMAPPacketIncrementMode or RMAPPacketNoIncrementMode
	 */
	inline void setIncrementMode(unsigned char mode){
		increment=(mode&0x01);
	}
	/** Setter for increment mode.
	 */
	inline void setIncrementMode(){
		increment=RMAPPacketIncrementMode;
	}
	/** Setter for increment mode.
	 */
	inline void setNoIncrementMode(){
		increment=RMAPPacketNoIncrementMode;
	}
	/** Getter for increment mode.
	 * @return an unsigned char value which holds RMAPPacketIncrementMode or RMAPPacketNoIncrementMode
	 */
	inline unsigned char getIncrementMode(){
		return increment;
	}
	/** Getter for increment mode.
	 * @return true if this packet is increment mode
	 */
	inline bool isIncrementMode() {
		if (increment==RMAPPacketIncrementMode) {
			return true;
		} else {
			return false;
		}
	}
	/** Getter for increment mode.
	 * @return true if this packet is non increment mode
	 */
	inline bool isNoIncrementMode() {
		if (increment==RMAPPacketNoIncrementMode) {
			return true;
		} else {
			return false;
		}
	}
	
	/** RMAP Source Path Address Length (2bit).
	 * The number of 32-bit boxes which are needed to
	 * put the Source Path Address. For example,
	 * if the source path address is "1,5,2,3,1",
	 * then two 32-bit boxes are needed. The boxes are
	 * padded with 0 if the length of the source path
	 * address is not multiple number of 4.
	 * In the example case, the Source Path Address field
	 * of an RMAP packet is set to "0,0,0,1" and "5,2,3,1". 
	 * The two least significant bits (LSB) of unsigned char
	 * type will be used.
	 */
	inline unsigned char getSourcePathAddressLength(){
		int sourceAddressVectorSize=destination.getSourcePathAddress().size();
		if(sourceAddressVectorSize==0){
			return 0;
		}else{
			return ((unsigned char)((sourceAddressVectorSize+3)/4)&0x03); //The 2 LSBs are used.
		}
	}
	
	/** Setter for Transaction ID.
	 * @param tid an unsigned int type Transaction ID. The 16 LSBs are used
	 */
	inline void setTransactionID(unsigned int tid){
		transactionID=tid;
	}
	/** Getter for Transaction ID.
	 * @return an unsigned int type Transaction ID.
	 */
	inline unsigned int getTransactionID(){
		return transactionID;
	}
	
	/** Setter for Extended Address.
	 * @param extendedaddress an unsigned char value for Extended Address.
	 */
	inline void setExtendedAddress(unsigned char extendedaddress){
		extendedAddress=extendedaddress;
	}
	/** Getter for Extended Address.
	 * @return an unsigned char value for Extended Address
	 */
	inline unsigned char getExtendedAddress() {
		return extendedAddress;
	}
	
	/** Setter for address of the remote node
	 * which this RMAP operation accesses.
	 * @param newaddress an unsigned int value for address. The 32 LSBs are used
	 */ 
	inline void setAddress(unsigned int newaddress){
		address=(newaddress&0xffffffff);
	}
	/** Getter for address.
	 * @return an unsigned int value for address
	 */
	inline unsigned int getAddress() {
		return address;
	}
	
	/** Setter for access length counted in a unit of byte (8-bit).
	 * Data length will also be set in setData() method.
	 * @param newlength an unsinged int value for length.
	 */ 
	inline void setLength(unsigned int newlength){
		length=(newlength&0x00ffffff); //The 24 LSBs are used.
	}
	/** Getter for access length counted in a unit of byte (8-bit).
	 * @return an unsinged int value for length.
	 */ 
	inline unsigned int getLength() {
		return length;
	}
	
	/** Setter for CRC type.
	 * CRC type of RMAP Draft version E and F are currently available.
	 * @param newcrctype an unsigned char value which holds RMAPPacketCRCDraftE or RMAPPacketCRCDraftF
	 */ 
	inline void setCRCType(unsigned char newcrctype){
		destination.setCRCType(newcrctype);
	}
	/** Getter for CRC type.
	 * @return an unsigned char value which holds RMAPPacketCRCDraftE or RMAPPacketCRCDraftF
	 */ 
	inline unsigned char getCRCType(){
		return destination.getCRCType();
	}

	/** Setter for EOP type.
	 * @param neweoptype an unsigned char value which holds RMAPPacket::EOP or RMAPPacket::EEP.
	 */ 
	inline void setEOPType(unsigned char neweoptype){
		eopType=neweoptype;
	}
	/** Getter for EOP type.
	 * @return an unsigned char value which holds RMAPPacket::EOP or RMAPPacket::EEP.
	 */ 
	inline unsigned char getEOPType(){
		return eopType;
	}

public:
	/** RMAP Protocol Identifier (8bits).
	 * fixed to 0x01 by definition.
	 */
	static const unsigned char RMAPProtocolIdentifier=0x01;
	static const unsigned char RMAPDefaultPacketType=0x01;
	static const unsigned char RMAPDefaultWriteOrRead=0x01;
	static const unsigned char RMAPDefaultVerifyMode=0x01;
	static const unsigned char RMAPDefaultAckMode=0x01;
	static const unsigned char RMAPDefaultIncrementMode=0x01;
	static const unsigned int RMAPDefaultTID=0x00;
	static const unsigned char RMAPDefaultExtendedAddress=0x00;
	static const unsigned int RMAPDefaultAddress=0x00;
	static const unsigned int RMAPDefaultLength=0x00;
	static const unsigned char RMAPDefaultHeaderCRC=0x00;
	static const unsigned char RMAPDefaultDataCRC=0x00; 
	static const unsigned char RMAPDefaultReplyStatus=0x00;
	
	static const unsigned char RMAPPacketTypeCommand=0x01;
	static const unsigned char RMAPPacketTypeReply=0x00;
	
	static const unsigned char RMAPPacketWriteMode=0x01;
	static const unsigned char RMAPPacketReadMode=0x00;
	
	static const unsigned char RMAPPacketVerifyMode=0x01;
	static const unsigned char RMAPPacketNoVerifyMode=0x00;
	
	static const unsigned char RMAPPacketAckMode=0x01;
	static const unsigned char RMAPPacketNoAckMode=0x00;
	
	static const unsigned char RMAPPacketIncrementMode=0x01;
	static const unsigned char RMAPPacketNoIncrementMode=0x00;
	
	static const unsigned char RMAPPacketCRCDraftE=0x00;
	static const unsigned char RMAPPacketCRCDraftF=0x01;
	
	static const bool RMAPDefaultCRCCheckMode=true;
	
	static enum {
		CommandExcecutedSuccessfully=0x00,
		GeneralError=0x01,
		UnusedRMAPPacketTypeOrCommandCode=0x02,
		InvalidDestinationKey=0x03,
		InvalidDataCRC=0x04,
		EarlyEOP=0x05,
		CargoTooLarge=0x06,
		EEP=0x07,
		VerifyBufferOverrun=0x09,
		CommandNotImplementedOrNotAuthorized=0x0a,
		RMWDataLengthError=0x0b,
		InvalidDataDestinationLogicalAddress=0x0c
	} RMAPReplyStatus;
	
	static enum {
		EOPTypeEOP,
		EOPTypeEEP
	} RMAPEOPType;
};

#endif /*RMAPPACKET_HH_*/
