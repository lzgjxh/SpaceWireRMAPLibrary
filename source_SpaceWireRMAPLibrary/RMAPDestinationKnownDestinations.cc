#include "RMAPDestinationKnownDestinations.hh"

using namespace std;

RMAPDestination* RMAPDestinationKnownDestinations::selectInstanceFromCommandLineMenu(){
	/** create new RMAPDestination instance
	 * old destination is reset here.
	 */
	RMAPDestination* rmapdestination=new RMAPDestination();

	int choice=0;

	//select how to set new RMAPDestination information
	setdestinationmenuchoice:
	cout << "Select : " << endl;
	cout << " 1 : Input new RMAP Destination" << endl;
	cout << " 2 : Reset to default RMAPDestination" << endl;
	cout << " 3 : Dump Current RMAP Destination" << endl;
	cout << " 4 : Quit RMAPDestination Setting Mode" << endl;

	//show frequently used RMAPDestination information
	cout << " ### Frequently Used Settings ###" << endl;
	cout << "  10 : Old Shimafuji SpaceWire IF Boards" << endl;
	cout << "       (Logical Address=0x01, Draft E CRC)"<< endl;
	cout << "  11 : Shimafuji SpaceWire IF Boards"<< endl;
	cout << "       (Logical Address=0xFE, Draft E CRC)"<< endl;
	cout << "  12 : Shimafuji SpaceWire IF Boards"<< endl;
	cout << "       (Logical Address=0xFE, Draft F CRC)"<< endl;
	cout << "  13 : Shimafuji SpaceCube Router Configuration Port"<< endl;
	cout << "       (Logical Address=0xFE, Draft E CRC, Destination Key=0x02)"<< endl;
	cout << "  14 : Shimafuji SpaceWire IF Boards (Shimafuji SpaceWire/RMAP IP)"<< endl;
	cout << "       (Logical Address=0x30, Draft E CRC, Destination Key=0x02)"<< endl;
	cout << "  15 : Shimafuji SpaceWire IF Boards (Shimafuji SpaceWire/RMAP IP)"<< endl;
	cout << "       (Logical Address=0x30, Draft F CRC, Destination Key=0x02)"<< endl;
	cout << "  20 : SpaceWireRouter by NEC"<< endl;
	cout << "  30 : SpaceWire Universal IO Board by MHI" << endl;
	cout << ">";
	cin >> dec >> choice;
	cout << endl;

	switch(choice){
	case 1:
		inputNewRMAPDestination(rmapdestination);
		break;
	case 2:
		setDefaultRMAPDestination(rmapdestination);
		break;
	case 3:
		rmapdestination->dump();
		break;
	case 4:
		return rmapdestination;
		break;
	case 10:
		setForSpaceWireIFBoardsByShimafujiDraftELogicalAddress0x02(rmapdestination);
		return rmapdestination;
		break;
	case 11:
		setForSpaceWireIFBoardsByShimafujiDraftELogicalAddress0xFE(rmapdestination);
		return rmapdestination;
		break;
	case 12:
		setForSpaceWireIFBoardsByShimafujiDraftFLogicalAddress0xFE(rmapdestination);
		return rmapdestination;
		break;
	case 13:
		setForSpaceCubeRouterConfigurationRegister(rmapdestination);
		return rmapdestination;
		break;
	case 14:
		setForSpaceWireIFBoardsByShimafujiDraftELogicalAddress0x30(rmapdestination);
		return rmapdestination;
		break;
	case 15:
		setForSpaceWireIFBoardsByShimafujiDraftFLogicalAddress0x30(rmapdestination);
		return rmapdestination;
		break;
	case 20:
		setForSpaceWireRouterByNEC(rmapdestination);
		return rmapdestination;
		break;
	case 30:
		setForSpaceWireUniversalIOBoardByMHI(rmapdestination);
		return rmapdestination;
		break;
	}
	goto setdestinationmenuchoice;
}
void RMAPDestinationKnownDestinations::inputNewRMAPDestination(RMAPDestination* rmapdestination){
	rmapdestination->setRMAPDestinationViaCommandLine();
	cout << "--- new RMAPDestination ---" << endl;
	rmapdestination->dump();
}
void RMAPDestinationKnownDestinations::setDefaultRMAPDestination(RMAPDestination* rmapdestination){
	rmapdestination->setDefaultDestination();
	cout << "Destination was set to default : " << endl;
	rmapdestination->dump();
}
void RMAPDestinationKnownDestinations::setForSpaceWireIFBoardsByShimafujiDraftELogicalAddress0x02(RMAPDestination* rmapdestination){
	rmapdestination->setDefaultDestination();
	rmapdestination->setDestinationLogicalAddress(0x01);
	rmapdestination->setDestinationKey(0x02);
	rmapdestination->setDraftECRC();
	vector<unsigned char> path;
	rmapdestination->setDestinationPathAddress(path);
	rmapdestination->setSourcePathAddress(path);
	rmapdestination->setSourceLogicalAddress(0xfe);
	rmapdestination->setWordWidth(1);
	rmapdestination->dump();
}
void RMAPDestinationKnownDestinations::setForSpaceWireIFBoardsByShimafujiDraftELogicalAddress0xFE(RMAPDestination* rmapdestination){
	rmapdestination->setDefaultDestination();
	rmapdestination->setDestinationLogicalAddress(0xFE);
	rmapdestination->setDestinationKey(0x02);
	rmapdestination->setDraftECRC();
	vector<unsigned char> path;
	rmapdestination->setDestinationPathAddress(path);
	rmapdestination->setSourcePathAddress(path);
	rmapdestination->setSourceLogicalAddress(0xfe);
	rmapdestination->setWordWidth(1);
	rmapdestination->dump();
}
void RMAPDestinationKnownDestinations::setForSpaceWireIFBoardsByShimafujiDraftFLogicalAddress0xFE(RMAPDestination* rmapdestination){
	rmapdestination->setDefaultDestination();
	rmapdestination->setDestinationLogicalAddress(0xFE);
	rmapdestination->setDestinationKey(0x02);
	rmapdestination->setDraftFCRC();
	vector<unsigned char> path;
	rmapdestination->setDestinationPathAddress(path);
	rmapdestination->setSourcePathAddress(path);
	rmapdestination->setSourceLogicalAddress(0xfe);
	rmapdestination->setWordWidth(1);
	rmapdestination->dump();
}
void RMAPDestinationKnownDestinations::setForSpaceWireIFBoardsByShimafujiDraftELogicalAddress0x30(RMAPDestination* rmapdestination){
	rmapdestination->setDefaultDestination();
	rmapdestination->setDestinationLogicalAddress(0x30);
	rmapdestination->setDestinationKey(0x02);
	rmapdestination->setDraftECRC();
	vector<unsigned char> path;
	rmapdestination->setDestinationPathAddress(path);
	rmapdestination->setSourcePathAddress(path);
	rmapdestination->setSourceLogicalAddress(0xfe);
	rmapdestination->setWordWidth(1);
	rmapdestination->dump();
}
void RMAPDestinationKnownDestinations::setForSpaceWireIFBoardsByShimafujiDraftFLogicalAddress0x30(RMAPDestination* rmapdestination){
	rmapdestination->setDefaultDestination();
	rmapdestination->setDestinationLogicalAddress(0x30);
	rmapdestination->setDestinationKey(0x02);
	rmapdestination->setDraftFCRC();
	vector<unsigned char> path;
	rmapdestination->setDestinationPathAddress(path);
	rmapdestination->setSourcePathAddress(path);
	rmapdestination->setSourceLogicalAddress(0xfe);
	rmapdestination->setWordWidth(1);
	rmapdestination->dump();
}
void RMAPDestinationKnownDestinations::setForSpaceCubeRouterConfigurationRegister(RMAPDestination* rmapdestination){
	rmapdestination->setDefaultDestination();
	rmapdestination->setDestinationLogicalAddress(0xFE);
	rmapdestination->setDestinationKey(0x02);
	rmapdestination->setDraftECRC();
	vector<unsigned char> path;
	path.push_back(0x00);
	rmapdestination->setDestinationPathAddress(path);
	path.clear();
	rmapdestination->setSourcePathAddress(path);
	rmapdestination->setSourceLogicalAddress(0xfe);
	rmapdestination->setWordWidth(4);
	rmapdestination->dump();
}
void RMAPDestinationKnownDestinations::setForSpaceWireRouterByNEC(RMAPDestination* rmapdestination){
	rmapdestination->setDefaultDestination();
	rmapdestination->setDestinationLogicalAddress(0xfe);
	rmapdestination->setDestinationKey(0x02);
	rmapdestination->setDraftECRC();
	vector<unsigned char> path;
	path.push_back(0x00);
	rmapdestination->setDestinationPathAddress(path);
	path.push_back(0x00);path.push_back(0x00);path.push_back(0x00);
	//rmapdestination->setSourcePathAddress(path);
	rmapdestination->setSourceLogicalAddress(0xfe);
	rmapdestination->setWordWidth(4);
	rmapdestination->dump();
}
void RMAPDestinationKnownDestinations::setForSpaceWireUniversalIOBoardByMHI(RMAPDestination* rmapdestination){
	cout << "Source Logical Address is set to 0x31" << endl;
	cout << "Please ensure the connection between SpaceCube and Universal IO Board" << endl;
	rmapdestination->setDefaultDestination();
	rmapdestination->setDestinationLogicalAddress(0x33);
	rmapdestination->setSourceLogicalAddress(0x31);
	rmapdestination->setDestinationKey(0x10);
	rmapdestination->setDraftFCRC();
	vector<unsigned char> path;
	path.clear();
	rmapdestination->setDestinationPathAddress(path);
	rmapdestination->setSourcePathAddress(path);
	rmapdestination->setWordWidth(2);
	rmapdestination->dump();
}

/** History
 * 2008-10-07 file created (Takayuki Yuasa)
 * 2009-01-21 comments added (Takayuki Yuasa)
 */
