#include "SpaceWireIFOverIPServer.hh"

#include <iostream>

#include <stdlib.h>
using namespace std;

class Main {
private:
	string hostname;
	unsigned int portnumber;
public:
	Main(unsigned int newportnumber) : 
		portnumber(newportnumber)
	{
		//Initialization
	}
public:
	void run(){
		//initialize
		SpaceWireIFOverIPServer* spacewireif=new SpaceWireIFOverIPServer(portnumber);
		spacewireif->initialize();

		for(unsigned char i=0;i<10;i++){
			cout << "Loop No." << (unsigned int)i << endl;
			//wait for acception
			cout << "waiting for a client SpaceWireIFOverIPClient...";
			spacewireif->open();
			cout << "accepted" << endl;

			//receive
			vector<unsigned char> receiveddata=spacewireif->receive();
			cout << receiveddata.size() << "byte(s) received" << endl;

			//send
			spacewireif->send(&receiveddata);

			//finalization
			spacewireif->close();
		}
	}
};

int main(int argc,char* argv[]){
	if(argc==2){
		unsigned int portnumber=atoi(argv[1]);
		Main main(portnumber);
		main.run();
	}else{
		cout << "give portnumber" << endl;
	}
	return 0;
}

/** History
 * 2008-08-26 file created (Takayuki Yuasa)
 */
