/** Sample Program For RMAPSocet class and 
 * RMAPEngine class.
 * 
 * *** Notification ***
 * To use this sample program correctly, you should
 * define an environmental flag "DEBUG" in 
 * SpaceWireIFConnectedToDummyRMAPTarget.cc.
 * More explicitly, add "#define DEBUG" and "#define DEBUG"
 * to the beginning of SpaceWireIFConnectedToDummyRMAPTarget.cc file.
 */

#include "SpaceWire.hh"
#include "RMAP.hh"

#include <util/tmonitor.h>

#include <iostream>
#include <iomanip>
using namespace std;

int main(int argc,char** argv){
	//tm_monitor();
	
	/** SpaceWireIF
	 */
	SpaceWireIF* spacewireif=SpaceWireIFImplementations::selectInstanceFromCommandLineMenu();
	spacewireif->initialize();
	spacewireif->open();
	cout << "spwid init" << endl;
	cout.flush();
	
	/** dummy RMAPDestination
	 */
	vector<unsigned char> paddress;
	unsigned int address=0x000000000;
	
	RMAPDestination dest;
	dest.setDestinationKey(0x02);
	cout << "destination constructed" << endl;cout.flush();
	
	/** RMAPSocket
	 */
	cout << "RMAPEngine init" << endl;cout.flush();
	RMAPEngine* rmapengine=new RMAPEngine(spacewireif);
	
	rmapengine->start();
	cout << "RMAPSocket init" << endl;cout.flush();
	RMAPSocket* rmapsocket=rmapengine->openRMAPSocketTo(dest);
	  // => set RMAPDestination to socket 

	/** Read Test
	 */
	cout << "######################" << endl;
	cout << "# Read Test" << endl;
	cout << "######################" << endl;
	vector<unsigned char>* readdata=rmapsocket->read(0x00000000,16);
	cout << "Read Result : ";
	SpaceWireUtilities::dumpPacket(readdata);

	/** Write Test
	 */
	cout << "######################" << endl;
	cout << "# Write Test" << endl;
	cout << "######################" << endl;
	vector<unsigned char> writedata;
	for(int i=0;i<0x16;i++){
		writedata.push_back((unsigned char)i);
	}
	rmapsocket->write(0x00000000,&writedata);
	
	Condition c;
	c.wait();
}