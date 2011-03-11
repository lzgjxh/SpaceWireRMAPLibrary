#include "SpaceWire.hh"
#include "RMAP.hh"

#include "Thread.hh"
#include "Condition.hh"
#include "Mutex.hh"

#include <iostream>
#include <vector>
using namespace std;

class Main {
public:
	Main(){
		//Initialization
	}
public:
	void run(){
		RMAPDestination* dest1=new RMAPDestination();
		vector<unsigned char> path;
		path.push_back(0x12);path.push_back(0x08);
		dest1->setDestinationPathAddress(path);
		
		RMAPDestination dest2=*dest1;
		RMAPDestination dest3;
		dest3=*dest1;
		
		dest1->dump();
		dest2.dump();
		dest3.dump();
		
		test(*dest1);
	}
	void test(RMAPDestination dest){
		cout << "method call" << endl;
		dest.dump();
		cout << "method call done" << endl;
	}
};

int main(int argc,char* argv[]){
	Main main;
	main.run();
	return 0;
}

/** History
 * 2008-10-08 file created (Takayuki Yuasa)
 */
