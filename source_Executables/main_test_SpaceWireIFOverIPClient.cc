#include "SpaceWireIFOverIPClient.hh"

#include <iostream>

#include <stdlib.h>
using namespace std;

class Main {
private:
	string hostname;
	unsigned int portnumber;
public:
	Main(string newhostname,unsigned int newportnumber) : 
		hostname(newhostname),
		portnumber(newportnumber)
	{
		//Initialization
	}
public:
	void run(){
		vector<unsigned char> data;
		for(unsigned char i=0;i<10;i++){
			cout << "Loop No." << (unsigned int)i << endl;
			
			//prepare data
			data.push_back(i);
			
			//initialize IF
			SpaceWireIF* spacewireif=new SpaceWireIFOverIPClient(hostname,portnumber);
			spacewireif->initialize();
			
			//connect to server IF
			spacewireif->open();
			
			//send
			spacewireif->send(&data);
			
			//receive
			vector<unsigned char> receiveddata=spacewireif->receive();
			cout << receiveddata.size() << "byte(s) received" << endl;
			
			//finalization
			spacewireif->close();
			delete spacewireif;
		}
	}
};

int main(int argc,char* argv[]){
	if(argc==3){
		string hostname=argv[1];
		unsigned int portnumber=atoi(argv[2]);
		Main main(hostname,portnumber);
		main.run();
	}else{
		cout << "give hostname and portnumber" << endl;
	}
	return 0;
}

/** History
 * 2008-08-26 file created (Takayuki Yuasa)
 */
