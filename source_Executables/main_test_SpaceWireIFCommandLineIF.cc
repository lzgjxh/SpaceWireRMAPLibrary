#include "SpaceWireIF.hh"
#include "SpaceWireIFCommandLineIF.hh"
#include "SpaceWireUtilities.hh"

#include <iostream>
#include <iomanip>
#include <vector>

#include <stdlib.h>
#include <string.h>
using namespace std;

int main(int argc,char** argv){
	SpaceWireIFCommandLineIF spacewireif;
	//initialize
	if(spacewireif.initialize()){
		cout << "SpaceWireIFCommandLineIF:successfully initialized" << endl;
	}else{
		cout << "initialize failed" << endl;
		exit(-1);
	}
	
	//open
	if(spacewireif.open()){
		cout << "SpaceWireIFCommandLineIF:successfully opend" << endl;
	}else{
		cout << "open failed" << endl;
		exit(-1);
	}
	
	//send
	cout << "SpaceWireIFCommandLineIF:send" << endl;
	vector<unsigned char> data;
	data.push_back(0x12);
	data.push_back(0x34);
	data.push_back(0x56);
	data.push_back(0x78);
	data.push_back(0x90);
	data.push_back(0xab);
	data.push_back(0xcd);
	data.push_back(0xef);
	spacewireif.send(&data);
	
	//receive
	cout << "SpaceWireIFCommandLineIF:receive" << endl;
	data.clear();
	data=spacewireif.receive();
	SpaceWireUtilities::dumpPacket(&data);
	
	//close
	spacewireif.close();
	
	//finalize
	spacewireif.finalize();
}
