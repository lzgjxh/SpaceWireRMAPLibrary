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
		portnumber(newportnumber) {
		//Initialization
	}
public:
	void run() {
		//initialize

		while (true) {
			try {
				SpaceWireIFOverIPServer* spacewireif = new SpaceWireIFOverIPServer(portnumber);
				spacewireif->initialize();
				spacewireif->open();
				cout << "waiting for iteration..." << endl;
				//receive packet size
				vector<unsigned char> data;
				data = spacewireif->receive();
				unsigned int packetSize = 0;
				for (int i = data.size() - 1; i >= 0; i--) {
					packetSize = packetSize * 0x100;
					packetSize += data.at(i);
				}
				cout << packetSize << endl;

				//receive iteration number
				data.clear();
				data = spacewireif->receive();
				unsigned int iterationNumber = 0;
				for (int i = data.size() - 1; i >= 0; i--) {
					iterationNumber = iterationNumber * 0x100;
					iterationNumber += data.at(i);
				}
				cout << iterationNumber << endl;

				//prepare data
				data.clear();
				for (unsigned int i = 0; i < packetSize; i++) {
					data.push_back(i % 0x100);
				}

				//send data
				cout << "started...";
				for (unsigned int i = 0; i < iterationNumber; i++) {
					spacewireif->send(&data);
				}
				cout << "done" << endl;
			} catch (SSDTPException e) {
				cout << "client exited..." << endl;
			}
		}
	}
};

int main(int argc, char* argv[]) {
	if (argc == 2) {
		unsigned int portnumber = atoi(argv[1]);
		Main main(portnumber);
		main.run();
	} else {
		cout << "give portnumber" << endl;
	}
	return 0;
}

/** History
 * 2008-08-26 file created (Takayuki Yuasa)
 */
