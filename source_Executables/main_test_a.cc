#include "SpaceWireIFViaSpW2TCPIPBridge.hh"
#include "Thread.hh"
#include "Condition.hh"

#include "IPServerSocket.hh"
#include "IPClientSocket.hh"

#include <iostream>
using namespace std;

class NodeB : public Thread {
public:
	NodeB() : Thread() {
		cout << "Node B instantiated" << endl;
	}
	void run(){
		this->sleep(1000);
		IPClientSocket datasocket("momi",SpaceWireIFViaSpW2TCPIPBridge::spw2tcpiphongo_send_dataport);
		IPClientSocket controlsocket("momi",SpaceWireIFViaSpW2TCPIPBridge::spw2tcpiphongo_send_controlport);
		datasocket.open();
		this->sleep(1000);
		controlsocket.open();
		
		SSDTPModule* ssdtpmodule=new SSDTPModule(&datasocket,&controlsocket);
		cout << "Node B is sending 16 byte data to Node A" << endl;cout.flush();
		vector<unsigned char>* data=new vector<unsigned char>();
		for(unsigned char i=0;i<16;i++){
			cout << "i" << (int)i << endl;cout.flush();
			data->push_back(i);
		}
		ssdtpmodule->send(*data);
		
		datasocket.close();
		controlsocket.close();
	}
};

int main(int argc,char* argv[]){
	cout << "SSDTPModule Test Program" << endl;
	cout << "Takayuki Yuasa et al." << endl << endl;
	NodeB* b=new NodeB();
	b->start();
	Condition c;
	c.wait(); //c is an instance of Condition class that is declared globally (see the top of this file)
	cout << "quitting..." << endl;
	b->exit();
}