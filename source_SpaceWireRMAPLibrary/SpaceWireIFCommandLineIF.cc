#include "SpaceWireIFCommandLineIF.hh"
using namespace std;

SpaceWireIFCommandLineIF::SpaceWireIFCommandLineIF() : SpaceWireIF(){}
SpaceWireIFCommandLineIF::~SpaceWireIFCommandLineIF(){}

bool SpaceWireIFCommandLineIF::initialize() throw(SpaceWireException){
	SpaceWireIF::initialize();
	timecode=0;
	return true;
}

bool SpaceWireIFCommandLineIF::open() throw(SpaceWireException){
	return true;
}

bool SpaceWireIFCommandLineIF::open(int portnumber) throw(SpaceWireException){
	return true;
}

void SpaceWireIFCommandLineIF::close(){}
void SpaceWireIFCommandLineIF::startLinkInitializationSequenceAsAMasterNode() throw(SpaceWireException){}
void SpaceWireIFCommandLineIF::startLinkInitializationSequenceAsASlaveNode() throw(SpaceWireException){}
void SpaceWireIFCommandLineIF::stopLink() throw(SpaceWireException){}

void SpaceWireIFCommandLineIF::resetStateMachine() throw(SpaceWireException){}
void SpaceWireIFCommandLineIF::clearSendFIFO() throw(SpaceWireException){}
void SpaceWireIFCommandLineIF::clearReceiveFIFO() throw(SpaceWireException){}

bool SpaceWireIFCommandLineIF::reset() throw(SpaceWireException){
	SpaceWireIF::reset();
	return true;
}

void SpaceWireIFCommandLineIF::finalize() throw(SpaceWireException){
	SpaceWireIF::finalize();
}

void SpaceWireIFCommandLineIF::send(vector<unsigned char>* packet) throw(SpaceWireException){
	int menu;
	cout << "SpaceWireIFCommandLineIF::send() invoked" << endl;
	cout << "Sent Packet is" << endl;
	SpaceWireUtilities::dumpPacket(packet);
	cout.flush();

	// selectsendmenu:
		menu=selectSendMenu();
	// switchselection:
		switch(menu){
		case 1: //save to file
			savePacketToFile(*packet);
			break;
		case 2:
			break;
		}
}

vector<unsigned char> SpaceWireIFCommandLineIF::receive() throw(SpaceWireException){
	int menu;
	cout << "SpaceWireIFCommandLineIF::receive() invoked" << endl;
	vector<unsigned char> dummyReply;
	selectreceivemenu:
		menu=selectReceiveMenu();
	// switchselection:
		switch(menu){
		 case 1:
			 dummyReply=commandLinePacketCreation();
			 break;
		 case 2:
			 dummyReply=fileStreamPacketCreation();
			 break;
		}

	if(dummyReply.size()==0){
		goto selectreceivemenu;
	}else{
		return dummyReply;
	}
}

void SpaceWireIFCommandLineIF::receive(vector<unsigned char>* packet) throw(SpaceWireException){
	*packet=receive();
}

void SpaceWireIFCommandLineIF::abort() throw(SpaceWireException){

}

SpaceWireAsynchronousStatus SpaceWireIFCommandLineIF::asynchronousReceive() throw(SpaceWireException){ return *(new SpaceWireAsynchronousStatus); }
SpaceWireAsynchronousStatus SpaceWireIFCommandLineIF::asynchronousSend(vector<unsigned char>* packet) throw(SpaceWireException){ return *(new SpaceWireAsynchronousStatus); }
void SpaceWireIFCommandLineIF::waitCompletionOfAsynchronousOperation(SpaceWireAsynchronousStatus status) throw(SpaceWireException){ }
//Send Menu
int SpaceWireIFCommandLineIF::selectSendMenu(){
	int menu;
	showmenu:
		cout << " 1 : Save to file" << endl;
		cout << " 2 : Do nothing" << endl;
		cout << ">";
	// select:
		cin >> dec >> menu;
		cout << endl;
	// judge:
		if(0<menu && menu<3){
			return menu;
		}else{
			goto showmenu;
		}
}

void SpaceWireIFCommandLineIF::savePacketToFile(vector<unsigned char> packet){
	string filename;
	cout << "(filename) >";
	cin >> filename;
	cout << endl;
	ofstream ofs(filename.c_str());
	SpaceWireUtilities::dumpPacket(&ofs,&packet);
	ofs.close();
	cout << "Savee to " << filename << endl;
}

//Receive Menu
int SpaceWireIFCommandLineIF::selectReceiveMenu(){
	int menu;
	showmenu:
		cout << "Choose the reply packet creation method." << endl;
		cout << " 1 : Direct Input" << endl;
		cout << " 2 : Read File" << endl;
		cout << ">";cout.flush();
	// select:
		cin >> menu;
		cout << endl;
	// judge:
		if(0<menu && menu<3){
			return menu;
		}else{
			goto showmenu;
		}
}

vector<unsigned char> SpaceWireIFCommandLineIF::commandLinePacketCreation(){
	cout << "Input data for 'Fake Reply Packet' in hexadecimal :" << endl;
	cout << "(to finish input, type a number greater than 0xFF)" << endl;
	int data=0x00;
	vector<unsigned char> dummyReply;
	int menu;

	inputdummypacket:
		dummyReply.clear();
		data=0x00;
		while(0<=data && data<0x100){
			cout << setw(4) << setfill('0') << dummyReply.size() << " >";
			cin >> hex >> data;
			if(0<data && data<0x100){
				dummyReply.push_back(data);
			}
		}
		cout << endl;
	confirmthenreturn:
		cout << "Entered Packet values are : " << endl;
		SpaceWireUtilities::dumpPacket(&dummyReply);
		cout << endl;
		cout << "Select : " << endl;
		cout << " 1 : OK" << endl;
		cout << " 2 : NG, re-input" << endl;
		cout << " 3 : Cancel Direct Input (back to menu)" << endl;
		cout << ">";
		cin >> menu;
		cout << endl;
		switch(menu){
		case 1:
			return dummyReply;
			break;
		case 2:
			goto inputdummypacket;
			break;
		case 3:
			dummyReply.clear();
			return dummyReply; //size zero
			break;
		default:
			goto confirmthenreturn;
			break;
		}
}

vector<unsigned char> SpaceWireIFCommandLineIF::fileStreamPacketCreation(){
	int menu;
	string filename;
	string str;
	unsigned char data;
	ifstream ifs;
	vector<unsigned char> dummyReply;

	inputfilename:
		dummyReply.clear();
		cout << "Read Dummy Packet from file (or go back to menu with 'exit')" << endl;
		cout << "(filename) >";
		cin >> filename;
		cout << endl;
		//check exit
		if(filename=="exit"){
			return dummyReply; //size zero
		}
		//check if file exists
		ifs.clear();
		ifs.open(filename.c_str());
		if(!ifs.is_open()){ //if not opened
			cout << "file not found..." << endl;
			cout << "please retry" << endl;
			goto inputfilename;
		}else{//opened
			cout << "read reply packet data from " << filename << endl;
		}
	// readfile:
		//read file
	while((ifs >> hex >> str)){
			data=SpaceWireUtilities::convertStringToUnsignedChar(str);
			dummyReply.push_back(data);
		}
		ifs.close();
	confirmthenreturn:
		cout << "Read Packet values are : " << endl;
		SpaceWireUtilities::dumpPacket(&dummyReply);
		cout << endl;
		cout << "Select : " << endl;
		cout << " 1 : OK" << endl;
		cout << " 2 : NG, re-enter" << endl;
		cout << " 3 : Cancel Direct Input (back to menu)" << endl;
		cout << ">";
		cin >> menu;
		cout << endl;
		switch(menu){
		case 1:
			return dummyReply;
			break;
		case 2:
			goto inputfilename;
			break;
		case 3:
			dummyReply.clear();
			return dummyReply; //size zero
			break;
		default:
			goto confirmthenreturn;
			break;
		}
		return dummyReply; //size zero
}

void SpaceWireIFCommandLineIF::sendTimeCode(unsigned char flag_and_timecode) throw(SpaceWireException){
	cout << "TimeCode : 0x" << setw(2) << setfill('0') << hex << flag_and_timecode << " Sent" << endl;
	timecode=flag_and_timecode;
}

unsigned char SpaceWireIFCommandLineIF::getTimeCode() throw(SpaceWireException){
	return timecode;
}
