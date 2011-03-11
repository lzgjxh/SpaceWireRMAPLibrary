#include "RMAPEngine.hh"
#include "SpaceWireIFSpaceCube1NECSoftIPCore.hh"

#include <iostream>
#include <iomanip>
using namespace std;

int main(int argc,char* argv[]){
	SpaceWireIF* spacewireif=new SpaceWireIFSpaceCube1NECSoftIPCore(0);
	RMAPEngine* rmapengine=new RMAPEngine(spacewireif);
	
	RMAPDestination spwdioboard; 
	RMAPSocket socket2dio=rmapengine.getSocket(spwdioboard);
	
	vector<unsigned char> data=socket2dio.read(0x01010000);
	for(int i=0;i<data.size();i++){
		cout << hex << setw(2) << (unsigned int)data.at(i) << endl;
	}
}