#include "RMAPDestination.hh"
using namespace std;

RMAPDestination::RMAPDestination(){
	setDefaultDestination();
}

void RMAPDestination::setDefaultDestination(){
	setDestinationLogicalAddress(RMAPDestination::DefaultLogicalAddress);
	setSourceLogicalAddress(RMAPDestination::DefaultLogicalAddress);
	clearDestinationPathAddress();
	clearSourcePathAddress();
	setSourcePathAddressLength(0x00);
	setDestinationKey(RMAPDestination::DefaultDestinationKey);
	setCRCType(RMAPDestination::DefaultCRCType);
	setWordWidth(RMAPDestination::DefaultWordWidth);
}

void RMAPDestination::dump(){
	//cout << "RMAPDestination::dump() invoked" << endl;
	//Destination
	cout << "Destination" << endl;
	cout << "  Logical Address   : " << setw(2) << setfill('0') << hex << (unsigned int)getDestinationLogicalAddress() << endl;
	cout << "  Path Address      : ";
	vector<unsigned char> destinationpathaddress=getDestinationPathAddress();
	if(destinationpathaddress.size()!=0){
		SpaceWireUtilities::dumpPacket(&destinationpathaddress);
	}else{
		cout << "none" << endl;
	}
	//Destination Key
	cout << "  Destination Key   : " << setw(2) << setfill('0') << hex << (unsigned int)getDestinationKey() << endl;
	//CRC Type
	cout << "  CRC Version       : " << ((getCRCType()==RMAPCRCTypeDraftE)? "Draft E":"Draft F") << endl;
	cout << "  Word Width        : " << (unsigned int)getWordWidth() << "bytes-1Word" << endl;
	cout << endl;
	
	//Source
	cout << "Source" << endl;
	cout << "  Logical Address   : " << setw(2) << setfill('0') << hex << (unsigned int)getSourceLogicalAddress() << endl;
	cout << "  Path Address      : ";
	vector<unsigned char> sourcepathaddress=getSourcePathAddress();
	if(sourcepathaddress.size()!=0){
		SpaceWireUtilities::dumpPacket(&sourcepathaddress);
	}else{
		cout << "none" << endl;
	}
	cout << "  Path Address Len. : " << setw(2) << setfill('0') << hex << (unsigned int)sourcePathAddressLength << endl;
	cout << dec << endl;
}

void RMAPDestination::setRMAPDestinationViaCommandLine(){
	//cout << "RMAPDestination::setRMAPDestinationViaCommandLine() invoked" << endl;

	unsigned int value;
	vector<unsigned char> path;
	SpaceWireCLI cli;
	
	//destination info
	cout << "---Destination Part---" << endl;
	// logical address
	cli.ask2_hex(cin,value,cout," Logical Address (hex) : ");
	setDestinationLogicalAddress((unsigned char)value);
	// path address
	path.clear();
	cout << " Path Address : (to terminate, please input a value greater than 0x100)" << endl;
	do{
		cout << "  [" << dec << path.size() << "] (hex) : ";
		cin >> hex >> value;
		if(value<0x100){
			path.push_back((unsigned char)value);
		}
	}while(value<0x100);
	setDestinationPathAddress(path);
	path.clear();
	// destination key
	cli.ask2_hex(cin,value,cout," Destination Key (hex) : ");
	setDestinationKey((unsigned char)value);
	// crc type
	crcselection:
	cout << " CRC Type :" << endl;
	cout << "  Draft E [1]" << endl;
	cout << "  Draft F [2]" << endl;
	cout << " 1 or 2 : ";
	cin >> hex >> value;
	if(value==0x01){
		setDraftECRC();
	}else if(value==0x02){
		setDraftFCRC();
	}else{
		goto crcselection;
	}
	// word width
	wordwidthloop:
	cout << " Word Width (in byte) : ";
	cin >> dec >> value;
	if(value!=0 && value<128){
		setWordWidth(value);
	}else{
		goto wordwidthloop;
	}
	
	//source info
	cout << "---Source Part---" << endl;
	// logical address
	cli.ask2_hex(cin,value,cout," Logical Address (hex) : ");
	setSourceLogicalAddress((unsigned char)value);
	// path address
	path.clear();
	cout << " Path Address : (to terminate, please input a value greater than 0x100)" << endl;
	do{
		cout << "  [" << dec << path.size() << "] (hex) : ";
		cin >> hex >> value;
		if(value<0x100){
			path.push_back((unsigned char)value);
		}
	}while(value<0x100);
	setSourcePathAddress(path);
	path.clear();
	// path address length
	spalselection:
	cout << " Source Path Address Length :" << endl;
	cout << "  Auto   [1]" << endl;
	cout << "  Manual [2]" << endl;
	cout << " 1 or 2 : ";
	cin >> hex >> value;
	if(value==0x01){
		setSourcePathAddressLength();
	}else if(value==0x02){
		spalinputloop:
		cout << " Source Path Address Length (0-3) : ";
		cin >> hex >> value;
		if(value<3){
			setSourcePathAddressLength((unsigned char)value);
		}else{
			goto spalinputloop;
		}
	}else{
		goto spalselection;
	}
	cout << dec << endl;
}
