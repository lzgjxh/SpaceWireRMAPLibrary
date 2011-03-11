#include "SpaceWire.hh"
#include "RMAP.hh"

#include <iostream>
using namespace std;

class Main {
public:
	Main(){
		//Initialization
	}
public:
	void run(){
		cout << "SpaceWireIF TimeCode Test" << endl;
		SpaceWireIF* spacewireif=SpaceWireIFImplementations::selectInstanceFromCommandLineMenu();
		spacewireif->initialize();
		spacewireif->open();

		cout << "Getting TimeCode" << endl;
		cout << "TimeCode is 0x" << setw(2) << setfill('0') << hex << (unsigned int)spacewireif->getTimeCode() << endl << endl;

		cout << "Input TimeCode and Flag to be sent" << endl;
		cout << "hex > ";
		unsigned char flag_and_timecode;
		cin >> hex >> flag_and_timecode;
		spacewireif->sendTimeCode(flag_and_timecode);
		cout << endl << endl;

		cout << "sending TimeCode 0x" << setw(2) << setfill('0') << hex << (unsigned int)flag_and_timecode << endl;

		cout << endl;
		cout << "Getting TimeCode" << endl;
		cout << "TimeCode is 0x" << setw(2) << setfill('0') << hex << (unsigned int)spacewireif->getTimeCode() << endl << endl;
	}
};

int main(int argc,char* argv[]){
	Main main;
	main.run();
	return 0;
}

/** History
 * 2008-12-17 file created (Takayuki Yuasa)
 */
