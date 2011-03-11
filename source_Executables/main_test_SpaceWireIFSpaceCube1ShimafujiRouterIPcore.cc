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
		cout << "SpaceCube Router IP Core Test" << endl;
		cout << " Note : This test program requires a SpaceWire connection between " << endl;
		cout << "        SpaceCube Router Port 1 (CN2) and Port 2 (CN3). Ensure that" << endl;
		cout << "        they are connected with a SpaceWire cable." << endl;
		cout << endl;
		
		cout << "#################################" << endl;
		cout << " Initialize" << endl;
		cout << "#################################" << endl;
		vector<unsigned char>* sentdata=new vector<unsigned char>();
		vector<unsigned char>* receiveddata=new vector<unsigned char>();
		
		cout << "Opening SpaceWire I/F Port 4..." << endl;
		SpaceWireIF* spacewireif4=new SpaceWireIFSpaceCube1ShimafujiRouterIPcore();//open port 4
		spacewireif4->initialize();
		spacewireif4->open(4);
		cout << "done" << endl << endl;
		
		cout << "Opening SpaceWire I/F Port 5..." << endl;
		SpaceWireIF* spacewireif5=new SpaceWireIFSpaceCube1ShimafujiRouterIPcore();//open port 5
		spacewireif5->initialize();
		spacewireif5->open(5);
		cout << "done" << endl << endl;

		
		cout << "#################################" << endl;
		cout << " Test 1 : Routing inside SpaceWire FPGA" << endl;
		cout << "#################################" << endl;
		cout << "send a packet from SpaceWire I/F Port 4 to Port 5" << endl;
		cout << "path address : 0x05" << endl;
		sentdata->clear();
		sentdata->push_back(0x05);
		for(unsigned char i=0;i<16;i++){
			sentdata->push_back(i);
		}
		cout << "Sent Packet containts are" << endl;
		SpaceWireUtilities::dumpPacket(sentdata);
		cout << "sending...";
		spacewireif4->send(sentdata);
		cout << "done" << endl << endl;

		cout << "wait a packet which comes to SpaceWire I/F Port 5" << endl;
		for(unsigned char i=0;i<16;i++){
			receiveddata->push_back(i);
		}
		cout << "waiting...";
		receiveddata->clear();
		spacewireif5->receive(receiveddata);
		cout << "done" << endl;
		cout << endl;
		cout << "Received Packet containts are" << endl;
		SpaceWireUtilities::dumpPacket(receiveddata);
		cout << endl;


		cout << "#################################" << endl;
		cout << " Test 2 : Routing goes outside SpaceWire FPGA" << endl;
		cout << "#################################" << endl;
		cout << "send a packet from SpaceWire I/F Port 4 to Port 5 through Port 1 and Port 2" << endl;
		cout << "path address : 0x01 0x05" << endl;
		sentdata->clear();
		sentdata->push_back(0x01);
		sentdata->push_back(0x05);
		for(unsigned char i=0;i<16;i++){
			sentdata->push_back(i);
		}
		cout << "Sent Packet containts are" << endl;
		SpaceWireUtilities::dumpPacket(sentdata);
		cout << "sending...";
		spacewireif4->send(sentdata);
		cout << "done" << endl << endl;

		cout << "wait a packet which comes to SpaceWire I/F Port 5" << endl;
		for(unsigned char i=0;i<16;i++){
			receiveddata->push_back(i);
		}
		cout << "waiting...";
		receiveddata->clear();
		spacewireif5->receive(receiveddata);
		cout << "done" << endl;
		cout << endl;
		cout << "Received Packet containts are" << endl;
		SpaceWireUtilities::dumpPacket(receiveddata);
		cout << endl;

		
		cout << "#################################" << endl;
		cout << " Test 3 : Circular Loop Routing " << endl;
		cout << "#################################" << endl;
		cout << "send a packet from SpaceWire I/F Port 4 to Port 5 through Port 1 and Port 2 many times" << endl;
		cout << "path address : 0x01 0x01 0x01 ... 0x01 0x05" << endl;
		sentdata->clear();
		for(int i=0;i<16;i++){
			sentdata->push_back(0x01);
		}
		sentdata->push_back(0x05);
		for(unsigned char i=0;i<16;i++){
			sentdata->push_back(i);
		}
		cout << "Sent Packet containts are" << endl;
		SpaceWireUtilities::dumpPacket(sentdata);
		cout << "sending...";
		spacewireif4->send(sentdata);
		cout << "done" << endl << endl;

		cout << "wait a packet which comes to SpaceWire I/F Port 5" << endl;
		for(unsigned char i=0;i<16;i++){
			receiveddata->push_back(i);
		}
		cout << "waiting...";
		receiveddata->clear();
		spacewireif5->receive(receiveddata);
		cout << "done" << endl;
		cout << endl;
		cout << "Received Packet containts are" << endl;
		SpaceWireUtilities::dumpPacket(receiveddata);
		cout << endl;
		
		cout << "#################################" << endl;
		cout << " Finalizatopn" << endl;
		cout << "#################################" << endl;
		cout << "Closing SpaceWire I/Fs" << endl;
		
		//finalization
		spacewireif4->close();
		delete spacewireif4;
		spacewireif5->close();
		delete spacewireif5;
	}
};

int main(int argc,char* argv[]){
	Main main;
	main.run();
	return 0;
}

/** History
 * 2008-10-03 file created (Takayuki Yuasa)
 */
