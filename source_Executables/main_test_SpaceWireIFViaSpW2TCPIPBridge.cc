#include "Condition.hh"

#include "SpaceWireIFViaSpW2TCPIPBridge.hh"

#include <iostream>
#include <string>
using namespace std;

class Main {
private:
	SpaceWireIF* spacewireif;
public:
	Main(){
		//Initialization
	}
public:
	void run(){
		cout << "========================================" << endl;
		cout << "SpaceWire to TCP/IP bridge Test Program" << endl;
		cout << "  Takayuki Yuasa et al. June 2008   " << endl;
		cout << endl;
		cout << "  This software is supported by     " << endl;
		cout << "  SpaceWire/RMAP 'Hongo' Library.   " << endl;
		cout << "========================================" << endl;
		cout << endl;
		
		cout << "input hostname on which sthongo (spw-tcpip bridge) is running : ";
		string hostname;
		cin >> hostname;
		cout << endl;
		
		cout << "initialize virtual SpaceWireIF" << endl;
		spacewireif=new SpaceWireIFViaSpW2TCPIPBridge(hostname);
		spacewireif->initialize();
		spacewireif->open();
		
		vector<unsigned char> data;
		for(unsigned char i=0;i<16;i++){
			data.push_back(i);
		}
		cout << "sending" << endl;
		spacewireif->send(&data);
		cout << "done" << endl;
		
		cout << "receiving" << endl;
		data=spacewireif->receive();
		cout << "done" << endl;
		for(unsigned char i=0;i<data.size();i++){
			cout << " " << (unsigned int)data.at(i);
		}
		cout << endl;
		
		Condition c;
		c.wait(3000);
		spacewireif->close();
		delete spacewireif;
	}
};

int main(int argc,char* argv[]){
	Main main;
	main.run();
	return 0;
}
