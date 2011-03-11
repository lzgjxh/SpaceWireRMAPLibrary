#include "SpaceWireIFImplementations.hh"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
using namespace std;

SpaceWireIF* SpaceWireIFImplementations::selectInstanceFromCommandLineMenu() throw(SpaceWireException){
	#ifdef SYSTEM_TYPE_POSIX
	cout << "SpaceWireIF OverIP is used." << endl;
	cout << "Input Hostname or IP address of SpaceWire-to-TCP/IP converter." << endl;
	cout << " note : IP port number can be provided separated by ':'; 133.11.165.117:10030" << endl;
	cout << " note : If no port number is given, the default " << SpaceWireIFOverIPServer::spw2tcpiphongo_dataport << " will be used." << endl;
	cout << "Hostname or IP address > ";
	string hostname;
	cin >> hostname;
	cout << endl;

	//port number
	size_t index=hostname.find(":");
	unsigned int portnumber=SpaceWireIFOverIPServer::spw2tcpiphongo_dataport;
	if(index!=string::npos){
		cout << ": is found at " << index << endl;
		string str_portnumber=hostname.substr(index+1);
		hostname=hostname.substr(0,index);
		portnumber=atoi(str_portnumber.c_str());
	}
	cout << "Hostname : " << hostname << endl;
	cout << "TCP/IP Portnumber : " << portnumber << endl;

	return new SpaceWireIFOverIPClient(hostname,portnumber);
	#endif

	#ifdef SYSTEM_TYPE_T_KERNEL
	cout << "Which SpaceWireIF implementation: " << endl;
	#ifdef USE_SPC1_NEC_IP_CORE
	cout << "  NEC Soft IP Core          [1]" << endl;
	#endif
	cout << "  Shimafuji IP Core         [2]" << endl;
	cout << "  Shimafuji Router IP Core  [3]" << endl;

	string str;
	int choice;
	select:
	SpaceWireCLI cli;
	unsigned int port;
	cli.ask2_int(cin,choice,cout,"select > ");
	if(0<choice && choice<4) {
		switch(choice){
			#ifdef USE_SPC1_NEC_IP_CORE
			case 1:
				return new SpaceWireIFSpaceCube1NECSoftIPcore();
				break;
			#endif
			case 2:
				portloop_SpaceWireIFSpaceCube1ShimafujiIPcore:
				cout << "Which port? [1,2,3] > ";
				cin >> port;

				if(port<1 || 3<port){
					goto portloop_SpaceWireIFSpaceCube1ShimafujiIPcore;
				}

				return new SpaceWireIFSpaceCube1ShimafujiIPcore(port);
				break;
			case 3:
				portloop_SpaceWireIFSpaceCube1ShimafujiRouterIPcore:
				cout << "which port? [4,5,6] > ";
				cin >> port;

				if(port<4 || 6<port){
					goto portloop_SpaceWireIFSpaceCube1ShimafujiRouterIPcore;
				}

				return new SpaceWireIFSpaceCube1ShimafujiRouterIPcore(port);
			default:
				break;
		}
	}else{
		goto select;
	}
	#endif
}

SpaceWireIF* SpaceWireIFImplementations::createInstanceFromName(string spacewireifname) throw(SpaceWireException){
#ifdef SYSTEM_TYPE_T_KERNEL
	//shimafuji ip
	string byte0_10=spacewireifname.substr(0,10);
	if(byte0_10=="shimafuji3"){
		unsigned int port=atoi(spacewireifname.substr(15,1).c_str());
		if(1<=port && port<=3){
			//cout << "SpaceWire Interface '" << spacewireifname << "' is opened." << endl;
			SpaceWireIFSpaceCube1ShimafujiIPcore* a=new SpaceWireIFSpaceCube1ShimafujiIPcore(port);
			return a;
		}else{
			throw SpaceWireException(SpaceWireException::NoDevice);
		}
	}

	string byte0_16=spacewireifname.substr(0,16);
	if(byte0_16=="shimafuji_router"){
		unsigned int port=atoi(spacewireifname.substr(21,1).c_str());
		if(4<=port && port<=6){
			//cout << "SpaceWire Interface '" << spacewireifname << "' is opened." << endl;
			return (SpaceWireIF*)(new SpaceWireIFSpaceCube1ShimafujiRouterIPcore(port));
		}else{
			throw SpaceWireException(SpaceWireException::NoDevice);
		}
	}
#endif

	//no match
	throw SpaceWireException(SpaceWireException::NoDevice);
}

/** History
 * 2008-09-02 file created (Takayuki Yuasa)
 * 2008-09-04 ifdef senetences are added for NEC Soft IP core (Takayuki Yuasa)
 * 2008-10-30 modified for Shimafuji 3port version IP Core (Takayuki Yuasa)
 * 2008-12-05 createInstanceFromName() added (Takayuki Yuasa)
 */
