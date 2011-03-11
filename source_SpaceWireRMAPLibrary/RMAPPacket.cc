#include "RMAPPacket.hh"
using namespace std;

/** Creates an RMAP packet based on the established parameters.
 */
void RMAPPacket::createPacket(){
	createHeader();
	//clear packet variable
	packet->clear();
	//Destination Path Address
	int length;
	vector<unsigned char> tmpvector;
	if(isCommandPacket()){
		tmpvector=destination.getDestinationPathAddress();
	}else{
		//Since no source path address starts with 0x00,
		//here, heading 0x00s are eliminated.
		vector<unsigned char> tmpvector2=destination.getSourcePathAddress();
		if(tmpvector2.size()!=0){
			unsigned int i=0;
			try{
				while(tmpvector2.at(i)==0x00){
					i++;
				}
			}catch(...){
				throw RMAPException(RMAPException::InvalidDestination);
			}
			for(;i<tmpvector2.size();i++){
				tmpvector.push_back(tmpvector2.at(i));
			}
		}
	}
	length=tmpvector.size();
	for(int i=0;i<length;i++){
		packet->push_back(tmpvector.at(i));
	}

	//Header
	for(unsigned int i=0;i<header.size();i++){
		packet->push_back(header.at(i));
	}

	//Header CRC
	packet->push_back(getHeaderCRC());

	//if Write
	if((isCommandPacket() && isWriteMode()) || //if write command
			(isReplyPacket() && isReadMode()) ){  //or if read reply
		//Data part should be connected after the Header part
		for (unsigned int i=0; i<data.size(); i++) {
			packet->push_back(data.at(i));
		}
		//Data CRC
		packet->push_back(getDataCRC());
	}
}

void RMAPPacket::createHeader(){
	header.clear();
	if(isCommandPacket()){
		//Create Command Packet Header
		//Header Part
		//Destination Logical Address
		header.push_back(destination.getDestinationLogicalAddress());
		//Protocol Identifier
		header.push_back((unsigned char)RMAPPacket::RMAPProtocolIdentifier);
		//Packet Type, Command, Source Path Address Length
		unsigned char pt_c_spal=getPacketTypeCommandSPAL_for_packetCreation();
		header.push_back(pt_c_spal);
		//Destination Key
		header.push_back(destination.getDestinationKey());
		//Source Path Address
		vector<unsigned char> tmpvector;
		tmpvector=destination.getSourcePathAddress();
		if(tmpvector.size()!=0){
			//zero padding
			for(unsigned int i=0;i<((tmpvector.size()+3)/4)*4-tmpvector.size();i++){
				header.push_back(0x00);
			}
			for(unsigned int i=0;i<tmpvector.size();i++){
				header.push_back(tmpvector.at(i));
			}
		}
		//Source Logical Address
		header.push_back(destination.getSourceLogicalAddress());
		//Transaction ID
		header.push_back((unsigned char)((transactionID&0xff00)>>8));
		header.push_back((unsigned char)((transactionID&0x00ff)>>0));
		//Extended Address
		header.push_back(extendedAddress);
		//Address
		header.push_back((unsigned char)((address&0xff000000)>>24));
		header.push_back((unsigned char)((address&0x00ff0000)>>16));
		header.push_back((unsigned char)((address&0x0000ff00)>>8));
		header.push_back((unsigned char)((address&0x000000ff)>>0));
		//Data Length
		header.push_back((unsigned char)((length&0x00ff0000)>>16));
		header.push_back((unsigned char)((length&0x0000ff00)>>8));
		header.push_back((unsigned char)((length&0x000000ff)>>0));
	}else{
		//Create Reply Packet Header
		//Source Logical Address
		header.push_back(destination.getSourceLogicalAddress());
		//Protocol Identifier
		header.push_back((unsigned char)RMAPPacket::RMAPProtocolIdentifier);
		//Packet Type, Command, Source Path Address Length
		unsigned char pt_c_spal=getPacketTypeCommandSPAL_for_packetCreation();
		header.push_back(pt_c_spal);
		//Reply Status
		header.push_back(replyStatus);
		//Destination Logical Address
		header.push_back(destination.getDestinationLogicalAddress());
		//Transaction ID
		header.push_back((unsigned char)((transactionID&0xff00)>>8));
		header.push_back((unsigned char)((transactionID&0x00ff)>>0));
		if(isReadMode()){
			//if Read Reply
			header.push_back(0); //Reseved Byte
			//Data Length
			header.push_back((unsigned char)((length&0x00ff0000)>>16));
			header.push_back((unsigned char)((length&0x0000ff00)>>8));
			header.push_back((unsigned char)((length&0x000000ff)>>0));
		}
	}
}

vector<unsigned char>* RMAPPacket::getPacket(){
	return packet;
}

void RMAPPacket::interpretPacket() throw(RMAPException){
	try{
		//index
		int i=0;
		/** rmapIndex is an index of the Destination Logical Address
		 * item in a packet array.
		 * rmapIndexAfterSourcePathAddress is an index of the Souce
		 * Logical Address item in a packet array. dataIndex is an
		 * index of the first byte of data part in a packet array.
		 */
		int rmapIndex=0;
		int rmapIndexAfterSourcePathAddress=0;
		int dataIndex=0;

		/** Header interpretation
		 */

		/** If Path Address remains, strip them.
		 * (Path Address is less than 32.)
		 */
		tmppathaddress.clear();
		while((*packet)[i]<0x20){
			tmppathaddress.push_back((*packet)[i]);
			i++;
		}
		rmapIndex=i;
		/** Protocol Identifier.
		 * check if this packet is an RMAP packet->
		 * if not, throw an exception.
		*/
		if((*packet)[rmapIndex+1]!=RMAPProtocolIdentifier){
#ifdef DEBUG
			cerr << "RMAPPacket : RMAP Protocol Identifier mismatch" << endl;
			cerr << "throws RMAPException::NotAnRMAPPacket" << endl;
#endif
#ifdef DUMP
			SpaceWireUtilities::dumpPacket(packet);
#endif
			throw(RMAPException(RMAPException::NotAnRMAPPacket));
		}

		/** Packet Type, Command, Source Path Address Length
		 */
		unsigned char pt_c_spal=(*packet)[rmapIndex+2];

		//Command or Reply
		if((pt_c_spal&0x40)/0x40==RMAPPacketTypeCommand){
			setCommandPacket();
		}else{
			setReplyPacket();
		}
		//Write or Read
		if((pt_c_spal&0x20)/0x20==RMAPPacketWriteMode){
			setWriteMode();
		}else{
			setReadMode();
		}
		//Verify or Non Verify
		if((pt_c_spal&0x10)/0x10==RMAPPacketVerifyMode){
			setVerifyMode();
		}else{
			setNoVerifyMode();
		}
		//Ack or No Ack
		if((pt_c_spal&0x08)/0x08==RMAPPacketAckMode){
			setAckMode();
		}else{
			setNoAckMode();
		}
		//Increment or No Increment
		if((pt_c_spal&0x04)/0x04==RMAPPacketIncrementMode){
			setIncrementMode();
		}else{
			setNoIncrementMode();
		}
		//Source Path Address Length
		unsigned int spal=(unsigned int)((pt_c_spal&0x03)/0x01);
		destination.setSourcePathAddressLength((unsigned char)spal);

		/** Command or Reply
		 */
		if(isCommandPacket()){
			//Destination Path Address
			destination.setDestinationPathAddress(tmppathaddress);
			//Destination Logical Address
			destination.setDestinationLogicalAddress((*packet)[rmapIndex+0]);
			//Destination Key
			destination.setDestinationKey((*packet)[rmapIndex+3]);
			//Source Path Address
			vector<unsigned char> tmpvector;
			for(unsigned int i=0;i<spal*4;i++){
				tmpvector.push_back((*packet)[rmapIndex+4+i]);
			}
			destination.setSourcePathAddress(tmpvector);
			rmapIndexAfterSourcePathAddress=rmapIndex+4+spal*4;
			//Source Logical Address
			destination.setSourceLogicalAddress((*packet)[rmapIndexAfterSourcePathAddress+0]);
			//TransactionID
			unsigned char uppertid,lowertid;
			uppertid=(*packet)[rmapIndexAfterSourcePathAddress+1];
			lowertid=(*packet)[rmapIndexAfterSourcePathAddress+2];
			setTransactionID((unsigned int)(uppertid*0x100+lowertid));
			//Extended Address
			setExtendedAddress((*packet)[rmapIndexAfterSourcePathAddress+3]);
			//Write/Read Address
			unsigned char address_3,address_2,address_1,address_0;
			address_3=(*packet)[rmapIndexAfterSourcePathAddress+4];
			address_2=(*packet)[rmapIndexAfterSourcePathAddress+5];
			address_1=(*packet)[rmapIndexAfterSourcePathAddress+6];
			address_0=(*packet)[rmapIndexAfterSourcePathAddress+7];
			setAddress(address_3*0x01000000+address_2*0x00010000+address_1*0x00000100+address_0*0x00000001);
			//Data Length
			unsigned char length_2,length_1,length_0;
			length_2=(*packet)[rmapIndexAfterSourcePathAddress+8];
			length_1=(*packet)[rmapIndexAfterSourcePathAddress+9];
			length_0=(*packet)[rmapIndexAfterSourcePathAddress+10];
			setLength(length_2*0x010000+length_1*0x000100+length_0*0x000001);
			//compare Header CRC
			unsigned char hcrc=(*packet)[rmapIndexAfterSourcePathAddress+11];
			createHeader();
			if(crcCheckMode==true){
				if (getHeaderCRC()!=hcrc) {
					//CRC error
					//check another CRC version
					RMAPDestination tmpdestination;
					tmpdestination.setCRCType(RMAPDestination::RMAPCRCTypeDraftE);
					unsigned char draftecrc=RMAPUtilities::calcCRC(tmpdestination,*(this->getHeader()));
					tmpdestination.setCRCType(RMAPDestination::RMAPCRCTypeDraftF);
					unsigned char draftfcrc=RMAPUtilities::calcCRC(tmpdestination,*(this->getHeader()));
					if(draftecrc!=hcrc && draftfcrc!=hcrc){
						cout << "RMAPPacket::interpretPacket() RMAP Command Header Part" << endl;
						cout << "CRC (Draft E) " << hex << (unsigned int)draftecrc << " " << hex << (int)hcrc << endl;
						cout << "CRC (Draft F) " << hex << (unsigned int)draftfcrc << " " << hex << (int)hcrc << endl;
#ifdef DEBUG
						cout << "Whole Packet" << endl;
#ifdef DUMP
						SpaceWireUtilities::dumpPacket(packet);
						cout << endl;
						cout << "Header Part" << endl;
						SpaceWireUtilities::dumpPacket(&header);
#endif
#endif
						throw(RMAPException(RMAPException::InvalidHeaderCRC));
					}
				}
			}
			//data part
			dataIndex=rmapIndexAfterSourcePathAddress+12;
			data.clear();
			if(isWriteMode()){
				for(unsigned int i=0;i<getLength();i++){
					data.push_back((*packet)[dataIndex+i]);
				}
				//compare Data CRC
				unsigned char dcrc=(*packet)[dataIndex+getLength()];
				if(crcCheckMode==true){
					if(getDataCRC()!=dcrc){
						//CRC error
						//check another CRC version
						RMAPDestination tmpdestination;
						tmpdestination.setCRCType(RMAPDestination::RMAPCRCTypeDraftE);
						unsigned char draftecrc=RMAPUtilities::calcCRC(tmpdestination,*(this->getData()));
						tmpdestination.setCRCType(RMAPDestination::RMAPCRCTypeDraftF);
						unsigned char draftfcrc=RMAPUtilities::calcCRC(tmpdestination,*(this->getData()));
						if(draftecrc!=dcrc && draftfcrc!=dcrc){
							cout << "RMAPPacket::interpretPacket() Write Command Data Part" << endl;
							cout << "CRC (Draft E) " << hex << (unsigned int)draftecrc << " " << hex << (int)hcrc << endl;
							cout << "CRC (Draft F) " << hex << (unsigned int)draftfcrc << " " << hex << (int)hcrc << endl;
#ifdef DEBUG
#ifdef DUMP
							cout << "Whole Packet" << endl;
							SpaceWireUtilities::dumpPacket(packet);
							cout << endl;
							cout << "Header Part" << endl;
							SpaceWireUtilities::dumpPacket(&header);
#endif
#endif
							throw(RMAPException(RMAPException::InvalidHeaderCRC));
						}
					}
				}
			}
		}else{
			//Reply Packet
			//Source Path Address (fake based on received "sourcePathAddressLength" value)
			tmppathaddress.clear();
			for(unsigned int i=0;i<spal*4;i++){
				tmppathaddress.push_back(0);
			}
			destination.setSourcePathAddress(tmppathaddress);
			//Source Logical Address
			destination.setSourceLogicalAddress((*packet)[rmapIndex+0]);
			//Status
			setReplyStatus((*packet)[rmapIndex+3]);
			//Destination Logical Address
			destination.setDestinationLogicalAddress((*packet)[rmapIndex+4]);
			//TransactionID
			unsigned char uppertid,lowertid;
			uppertid=(*packet)[rmapIndex+5];
			lowertid=(*packet)[rmapIndex+6];
			setTransactionID((unsigned int)(uppertid*0x100+lowertid));
			if(isWriteMode()){
				//if Write Ack
				//compare Header CRC
				unsigned char hcrc=(*packet)[rmapIndex+7];
				createHeader();
				if(crcCheckMode==true){
					if (getHeaderCRC()!=hcrc) {
						//CRC error
						//check another CRC version
						RMAPDestination tmpdestination;
						tmpdestination.setCRCType(RMAPDestination::RMAPCRCTypeDraftE);
						unsigned char draftecrc=RMAPUtilities::calcCRC(tmpdestination,*(this->getHeader()));
						tmpdestination.setCRCType(RMAPDestination::RMAPCRCTypeDraftF);
						unsigned char draftfcrc=RMAPUtilities::calcCRC(tmpdestination,*(this->getHeader()));
						if(draftecrc!=hcrc && draftfcrc!=hcrc){
							cout << "RMAPPacket::interpretPacket() Write Reply Data Part" << endl;
							cout << "CRC (Draft E) " << hex << (unsigned int)draftecrc << " " << hex << (int)hcrc << endl;
							cout << "CRC (Draft F) " << hex << (unsigned int)draftfcrc << " " << hex << (int)hcrc << endl;
#ifdef DEBUG
#ifdef DUMP
							cout << "Whole Packet" << endl;
							SpaceWireUtilities::dumpPacket(packet);
							cout << endl;
							cout << "Header Part" << endl;
							SpaceWireUtilities::dumpPacket(&header);
#endif
#endif
							throw(RMAPException(RMAPException::InvalidHeaderCRC));
						}
					}
				}
			}else{
				//if Read Reply
				//Data Length
				unsigned char length_2,length_1,length_0;
				length_2=(*packet)[rmapIndex+8];
				length_1=(*packet)[rmapIndex+9];
				length_0=(*packet)[rmapIndex+10];
				setLength(length_2*0x010000+length_1*0x000100+length_0*0x000001);
				//compare Header CRC
				unsigned char hcrc=(*packet)[rmapIndex+11];
				createHeader();
				if(crcCheckMode==true){
					if (getHeaderCRC()!=hcrc) {
						//CRC error
						//check another CRC version
						RMAPDestination tmpdestination;
						tmpdestination.setCRCType(RMAPDestination::RMAPCRCTypeDraftE);
						unsigned char draftecrc=RMAPUtilities::calcCRC(tmpdestination,*(this->getHeader()));
						tmpdestination.setCRCType(RMAPDestination::RMAPCRCTypeDraftF);
						unsigned char draftfcrc=RMAPUtilities::calcCRC(tmpdestination,*(this->getHeader()));
						if(draftecrc!=hcrc && draftfcrc!=hcrc){
							cout << "RMAPPacket::interpretPacket() Read Reply Header Part" << endl;
							cout << "CRC (Draft E) " << hex << (unsigned int)draftecrc << " " << hex << (int)hcrc << endl;
							cout << "CRC (Draft F) " << hex << (unsigned int)draftfcrc << " " << hex << (int)hcrc << endl;
#ifdef DEBUG
#ifdef DUMP
							cout << "Whole Packet" << endl;
							SpaceWireUtilities::dumpPacket(packet);
							cout << endl;
							cout << "Header Part" << endl;
							SpaceWireUtilities::dumpPacket(&header);
#endif
#endif
							throw(RMAPException(RMAPException::InvalidHeaderCRC));
						}
					}
				}
				//data part
				dataIndex=rmapIndex+12;
				data.clear();
				for(unsigned int i=0;i<getLength();i++){
					data.push_back((*packet)[dataIndex+i]);
				}
				//compare Data CRC
				unsigned char dcrc=(*packet)[dataIndex+getLength()];
				if(crcCheckMode==true){
					if(getDataCRC()!=dcrc){
						//CRC error
						//check another CRC version
						RMAPDestination tmpdestination;
						tmpdestination.setCRCType(RMAPDestination::RMAPCRCTypeDraftE);
						unsigned char draftecrc=RMAPUtilities::calcCRC(tmpdestination,*(this->getData()));
						tmpdestination.setCRCType(RMAPDestination::RMAPCRCTypeDraftF);
						unsigned char draftfcrc=RMAPUtilities::calcCRC(tmpdestination,*(this->getData()));
						if(draftecrc!=dcrc && draftfcrc!=dcrc){
							cout << "RMAPPacket::interpretPacket() Read Reply Data Part" << endl;
							cout << "CRC (Draft E) " << hex << (unsigned int)draftecrc << " " << hex << (int)hcrc << endl;
							cout << "CRC (Draft F) " << hex << (unsigned int)draftfcrc << " " << hex << (int)hcrc << endl;
#ifdef DEBUG
#ifdef DUMP
							cout << "Whole Packet" << endl;
							SpaceWireUtilities::dumpPacket(packet);
							cout << endl;
							cout << "Header Part" << endl;
							SpaceWireUtilities::dumpPacket(&header);
#endif
#endif
							throw(RMAPException(RMAPException::InvalidHeaderCRC));
						}
					}
				}
			}
		}
	}catch(exception e){
#ifdef DEBUG
		cerr << "RMAPPacket : this is not an RMAP packet->" << endl;
		cerr << "throws RMAPException::NotAnRMAPPacket" << endl;
#ifdef DUMP
		SpaceWireUtilities::dumpPacket(packet);
#endif
#endif
		throw(RMAPException(RMAPException::NotAnRMAPPacket));
	}
}


void RMAPPacket::setPacket(vector<unsigned char>* newpacket) throw(RMAPException){
	*packet=*newpacket; //copy byte-to-byte
	interpretPacket();
}

void RMAPPacket::setPacketBuffer(vector<unsigned char>* newpacketbuffer) throw(RMAPException){
	delete packet;
	packet=newpacketbuffer; //change reference
	interpretPacket();
}

vector<unsigned char>* RMAPPacket::getPacketBuffer() throw(RMAPException){
	return packet;
}

void RMAPPacket::dump(){
	dumpPacket();
}

void RMAPPacket::dumpPacket(){
	cout << "RMAPPacket::dumpPacket() invoked" << endl;
	createPacket();
	cout << "interpreting packet...";cout.flush();
	try{
		interpretPacket();
	}catch(RMAPException e){
		cerr << "Exception at packet interprtation." << endl;
		cerr << "dump raw packet data" << endl;cout.flush();
		SpaceWireUtilities::dumpPacket(getPacket());
		return;
	}
	cout << "done" << endl;

	///////////////////////////////
	//Command or Reply
	///////////////////////////////
	if(isCommandPacket()){
		dumpCommandPacket();
	}else{
		dumpReplyPacket();
	}
}

void RMAPPacket::dumpCommandPacket(){
	///////////////////////////////
	//Command
	///////////////////////////////
	cout << "RMAP Packet Dump" << endl;
	//Path Address
	if(destination.getDestinationPathAddress().size()!=0){
		cout << "--------- Destination Path Address ---------" << endl;
		vector<unsigned char> destinationpathaddress=destination.getDestinationPathAddress();
		SpaceWireUtilities::dumpPacket(&destinationpathaddress);
	}

	//Header
	cout << "--------- RMAP Header Part ---------" << endl;
	//Destination Logical Address
	cout << "Destination Logic. Address: " << setw(2) << setfill('0') << hex << (unsigned int)destination.getDestinationLogicalAddress() << endl;
	//Protocol Identifier
	cout << "Protocol ID               : " << setw(2) << setfill('0') << hex << (unsigned int)1 << endl;
	//PT,C,SPAL
	cout << "P.Type, Command, S.P.A.L. : " << setw(2) << setfill('0') << hex << (unsigned int)getPacketTypeCommandSPAL_for_packetInterpretation() << endl;
	//packet type (Command or Reply)
	cout << " ------------------------------" << endl;
	cout << " |Reserved    : 0" << endl;
	cout << " |Packet Type : " << hex << setw(1) << setfill('0') << (unsigned int)commandOrReply;
	cout << " " << ((commandOrReply==1)? "(Command)":"(Reply)") << endl;
	//Write or Read
	cout << " |Write/Read  : " << hex <<setw(1) << setfill('0') << hex << (unsigned int)writeOrRead;
	cout << " " << ((writeOrRead==1)? "(Write)":"(Read)") << endl;
	//Verify mode
	cout << " |Verify Mode : " << setw(1) << setfill('0') << hex << (unsigned int)verify;
	cout << " " << ((verify==1)? "(Verify)":"(No Verify)") << endl;
	//Ack mode
	cout << " |  Ack Mode  : " << setw(1) << setfill('0') << hex << (unsigned int)ack;
	cout << " " << ((ack==1)? "(Ack)":"(No Ack)") << endl;
	//Increment
	cout << " | Increment  : " << setw(1) << setfill('0') << hex << (unsigned int)increment;
	cout << " " << ((increment==1)? "(Increment)":"(No Increment)") << endl;
	//SPAL
	cout << " | S.P.A.L.   : " << setw(2) << setfill('0') << hex << (unsigned int)getSourcePathAddressLength() << endl;
	cout << " | S.P.A.L. = Source Path Address Length" << endl;
	cout << " ------------------------------" << endl;
	cout << "Destination Key           : " << setw(2) << setfill('0') << hex << (unsigned int)destination.getDestinationKey() << endl;
	//Source Path Address
	if(destination.getSourcePathAddress().size()!=0){
		cout << "Source Path Address       : ";
		vector<unsigned char> sourcepathaddress=destination.getSourcePathAddress();
		SpaceWireUtilities::dumpPacket(&sourcepathaddress);
	}else{
		cout << "Source Path Address       : --none--" << endl;
	}
	cout << "Source Logical Address    : " << setw(2) << setfill('0') << hex << (unsigned int)destination.getSourceLogicalAddress() << endl;
	cout << "Transaction Identifier    : " << setw(2) << setfill('0') << hex << ((unsigned int)transactionID)/0x100 << " " << setw(2) << setfill('0') << hex << ((unsigned int)transactionID)%0x100 << endl;
	cout << "Extended Address          : " << setw(2) << setfill('0') << hex << (unsigned int)extendedAddress << endl;
	cout << "Address                   : " << setw(8) << setfill('0') << hex << (unsigned int)address << endl;
	cout << "Data Length (bytes)       : " << setw(6) << setfill('0') << hex << (unsigned int)length << endl;
	cout << "Header CRC                : " << setw(2) << setfill('0') << hex << (unsigned int)getHeaderCRC() << endl;

	//Data Part
	cout << "---------  RMAP Data Part  ---------" << endl;
	if(isWriteMode()){
		cout << "[data size = " << dec << (unsigned int)getLength() << "bytes]" << endl;
		SpaceWireUtilities::dumpPacket(&data);
		cout << "Data CRC                  : " << setw(2) << setfill('0') << hex << (unsigned int)getDataCRC() << endl;
	}else{
		cout << "--- none ---" << endl;
	}
	cout << endl;
	cout << "Total (bytes)             : " << dec << (unsigned int)getPacket()->size() << endl;
	cout << dec << endl;
}

void RMAPPacket::dumpReplyPacket(){
	///////////////////////////////
	//Reply
	///////////////////////////////
	cout << "RMAP Packet Dump" << endl;
	//Path Address
	if(destination.getSourcePathAddress().size()!=0){
		bool nonzeroflag=false;
		for(unsigned int i=0;i<destination.getSourcePathAddress().size();i++){
			if(destination.getSourcePathAddress().at(i)!=0){
				nonzeroflag=true;
			}
		}
		if(nonzeroflag){
			cout << "--------- Source Path Address ---------" << endl;
			cout << "Source Path Address       : ";
			vector<unsigned char> sourcepathaddress=destination.getSourcePathAddress();
			SpaceWireUtilities::dumpPacket(&sourcepathaddress);
		}
	}

	//Header
	cout << "--------- RMAP Header Part ---------" << endl;
	//Destination Logical Address
	cout << "Destination Logic. Address: " << setw(2) << setfill('0') << hex << (unsigned int)destination.getDestinationLogicalAddress() << endl;
	//Protocol Identifier
	cout << "Protocol ID               : " << setw(2) << setfill('0') << hex << (unsigned int)1 << endl;
	//PT,C,SPAL
	cout << "P.Type, Command, S.P.A.L. : " << setw(2) << setfill('0') << hex << (unsigned int)getPacketTypeCommandSPAL_for_packetInterpretation() << endl;
	//packet type (Command or Reply)
	cout << " ------------------------------" << endl;
	cout << " |Reserved    : 0" << endl;
	cout << " |Packet Type : " << hex << setw(1) << setfill('0') << (unsigned int)commandOrReply;
	cout << " " << ((commandOrReply==1)? "(Command)":"(Reply)") << endl;
	//Write or Read
	cout << " |Write/Read  : " << hex <<setw(1) << setfill('0') << hex << (unsigned int)writeOrRead;
	cout << " " << ((writeOrRead==1)? "(Write)":"(Read)") << endl;
	//Verify mode
	cout << " |Verify Mode : " << setw(1) << setfill('0') << hex << (unsigned int)verify;
	cout << " " << ((verify==1)? "(Verify)":"(No Verify)") << endl;
	//Ack mode
	cout << " |  Ack Mode  : " << setw(1) << setfill('0') << hex << (unsigned int)ack;
	cout << " " << ((ack==1)? "(Ack)":"(No Ack)") << endl;
	//Increment
	cout << " | Increment  : " << setw(1) << setfill('0') << hex << (unsigned int)increment;
	cout << " " << ((increment==1)? "(Increment)":"(No Increment)") << endl;
	//SPAL
	cout << " | S.P.A.L.   : " << setw(2) << setfill('0') << hex << (unsigned int)getSourcePathAddressLength() << endl;
	cout << " | S.P.A.L. = Source Path Address Length" << endl;
	cout << " ------------------------------" << endl;
	//Source Path Address

	string statusstring;
	switch(replyStatus){
	case 0x00: statusstring="Successfully Executed";break;
	case 0x01: statusstring="General Error";break;
	case 0x02: statusstring="Unused RMAP Packet Type or Command Code";break;
	case 0x03: statusstring="Invalid Destination Key";break;
	case 0x04: statusstring="Invalid Data CRC";break;
	case 0x05: statusstring="Early EOP";break;
	case 0x06: statusstring="Cargo Too Large";break;
	case 0x07: statusstring="EEP";break;
	case 0x08: statusstring="Reserved";break;
	case 0x09: statusstring="Verify Buffer Overrun";break;
	case 0x0a: statusstring="RMAP Command Not Implemented or Not Authorized";break;
	case 0x0b: statusstring="Invalid Destination Logical Address";break;
	default: statusstring="Reserved";break;
	}
	cout << "Status                    : " << setw(2) << setfill('0') << hex << (unsigned int)replyStatus << " (" << statusstring << ")" << endl;
	cout << "Dest. Logical Address     : " << setw(2) << setfill('0') << hex << (unsigned int)destination.getSourceLogicalAddress() << endl;
	cout << "Transaction Identifier    : " << setw(2) << setfill('0') << hex << ((unsigned int)transactionID)/0x100 << " " << setw(2) << setfill('0') << hex << ((unsigned int)transactionID)%0x100 << endl;
	if(isReadMode()){
		cout << "Data Length (bytes)       : " << setw(6) << setfill('0') << hex << (unsigned int)length << endl;
	}
	cout << "Header CRC                : " << setw(2) << setfill('0') << hex << (unsigned int)getHeaderCRC() << endl;

	//Data Part
	cout << "---------  RMAP Data Part  ---------" << endl;
	if(isReadMode()){
		cout << "[data size = " << dec << (unsigned int)getLength() << "bytes]" << endl;
		SpaceWireUtilities::dumpPacket(&data);
		cout << "Data CRC    : " << setw(2) << setfill('0') << hex << (unsigned int)dataCRC << endl;
	}else{
		cout << "--- none ---" << endl;
	}
	cout << endl;
	cout << "Total (bytes)             : " << dec << (unsigned int)getPacket()->size() << endl;
	cout << dec << endl;
}

/** History
 * 2008-05-xx file created (Takayuki Yuasa)
 */
