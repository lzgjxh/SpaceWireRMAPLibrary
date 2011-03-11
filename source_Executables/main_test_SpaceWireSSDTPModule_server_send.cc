#include "SpaceWire.hh"
#include "RMAP.hh"
#include "IPSocket.hh"

#include <iostream>

#include <stdlib.h>
using namespace std;

class Main {
public:
	static const int port=10023;
public:
	Main(){
		//Initialization
	}
public:
	void run(){
		IPServerSocket* listener=new IPServerSocket(10023);
		listener->open();
		loop:
		try{
			IPServerAcceptedSocket* socket;
			try{
				cout << "waiting a connection" << endl;
				socket=listener->accept();
				cout << "connected" << endl;
			}catch(IPSocketException e){
				cout << "accept() exception" << endl;
				::exit(-1);
			}
			SSDTPModule* ssdtp=new SSDTPModule(socket);

			vector<unsigned char> data;
			for(unsigned char i=0;i<16;i++){
				data.push_back(i);
			}
			cout << "sending a packet" << endl;
			ssdtp->send(data);
			cout << "done" << endl;

			cout << "sending a TimeCode" << endl;
			ssdtp->sendTimeCode(0x12);
			cout << "done" << endl;

			delete ssdtp;
			socket->close();
			delete socket;
		}catch(...){}
		goto loop;

	}
};

int main(int argc,char* argv[]){
	Main main;
	main.run();
	return 0;
}

/* History
 * 2008-12-25 file created (Takayuki Yuasa)
 */
