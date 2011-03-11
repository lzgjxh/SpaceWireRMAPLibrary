#include "SpaceWireUtilities.hh"
using namespace std;

void SpaceWireUtilities::dumpPacket(vector<unsigned char>* data) throw (SpaceWireException){
	dumpPacket(&cout,data,1);
}
	
void SpaceWireUtilities::dumpPacket(ostream* ofs,vector<unsigned char>* data) throw (SpaceWireException){
	dumpPacket(ofs,data,1);
}

void SpaceWireUtilities::dumpPacket(vector<unsigned char>* data,unsigned int wordwidth) throw (SpaceWireException){
	dumpPacket(&cout,data,wordwidth);
}

void SpaceWireUtilities::dumpPacket(ostream* ofs,vector<unsigned char>* data,unsigned int wordwidth) throw (SpaceWireException){
	int v;
	if(wordwidth==0){
		throw(SpaceWireException(SpaceWireException::SizeIncorrect));
	}
	if(data->size()%wordwidth!=0){
		throw(SpaceWireException(SpaceWireException::SizeIncorrect));
	}
	for(unsigned int i=0;i<(data->size()/wordwidth+SpaceWireUtilities::DumpsPerLine-1)/SpaceWireUtilities::DumpsPerLine;i++){
		for(unsigned int o=0;o<SpaceWireUtilities::DumpsPerLine;o++){
			if(i*SpaceWireUtilities::DumpsPerLine+o<data->size()/wordwidth){
				v=0;
				for(unsigned int p=0;p<wordwidth;p++){
					v=v+(int)data->at((i*SpaceWireUtilities::DumpsPerLine+o)*wordwidth+p);
				}
				(*ofs) << "0x" << hex << setw(2*wordwidth) << setfill('0') << v << "  ";
			}
		}
		(*ofs) << endl;
	}
}

unsigned char SpaceWireUtilities::convertStringToUnsignedChar(string str){
	int data=0;
	stringstream ss;
	if((str.at(0)=='0' && str.at(1)=='x') || (str.at(0)=='0' && str.at(1)=='X')){
		str=str.substr(2);
	}
	ss << str;
	ss >> hex >> data;
	return (unsigned char)data;
}

void SpaceWireUtilities::printVectorWithAddress(unsigned int address,vector<unsigned char>* data){
	cout << "Address    Data" << endl;
	for(unsigned int i=0;i<data->size();i++){
		cout << setfill('0') << setw(4) << hex << (address+i)/0x00010000;
		cout << "-" << setfill('0') << setw(4) << hex << (address+i)%0x00010000;
		cout << "  ";
		cout << "0x" << setfill('0') << setw(2) << hex << (unsigned int)(data->at(i)) << endl;
	}
}

void SpaceWireUtilities::printVectorWithAddress2bytes(unsigned int address,vector<unsigned char>* data){
	cout << "Address    Data" << endl;
	unsigned int size=data->size();
	if(size%2==0 && size!=0){
		for(unsigned int i=0;i<(size+1)/2;i++){
			cout << setfill('0') << setw(4) << hex << (address+i*2)/0x00010000;
			cout << "-" << setfill('0') << setw(4) << hex << (address+i*2)%0x00010000;
			cout << "  ";
			cout << "0x" << setfill('0') << setw(4) << hex << (unsigned int)(data->at(i*2+1)*0x100+data->at(i*2)) << endl;
		}
	}else if(size!=0){
		unsigned int i;
		for(i=0;i<size/2;i++){
			cout << setfill('0') << setw(4) << hex << (address+i*2)/0x00010000;
			cout << "-" << setfill('0') << setw(4) << hex << (address+i*2)%0x00010000;
			cout << "  ";
			cout << "0x" << setfill('0') << setw(4) << hex << (unsigned int)(data->at(i*2+1)*0x100+data->at(i*2)) << endl;
		}
		cout << setfill('0') << setw(4) << hex << (address+i*2)/0x00010000;
		cout << "-" << setfill('0') << setw(4) << hex << (address+i*2)%0x00010000;
		cout << "  ";
		cout << "0x??" << setfill('0') << setw(4) << hex << (unsigned int)(data->at(i*2)) << endl;
	}
}
