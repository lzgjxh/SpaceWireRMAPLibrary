#include "SpaceWire.hh"
#include "RMAP.hh"

#include "libspw.h"

#include <basic.h>
#include <btron/proctask.h>
#include <btron/util.h>

#include <vector>
#include <iostream>
using namespace std;

HANDLE spwhandler;
unsigned int ReceiveBufferSize=4000;

void receivetask(W arg){
	cout << "receive waiting" << endl;
	PVOID receivebuffer=Smalloc(ReceiveBufferSize);
	SPACEWIRE_STATUS status;
	SPACEWIRE_PACKET_PROPERTIES* properties=(SPACEWIRE_PACKET_PROPERTIES*)Smalloc(sizeof(SPACEWIRE_PACKET_PROPERTIES));
	SPACEWIRE_ID* packetid=(SPACEWIRE_ID*)Smalloc(sizeof(SPACEWIRE_ID));
	
	status=SpaceWire_ReadPackets(spwhandler,receivebuffer,ReceiveBufferSize,1,false,properties,packetid);
	status=SpaceWire_WaitOnReadCompleting(spwhandler,*packetid,true);
	
	if(status!=TRANSFER_SUCCESS){
		throw(SpaceWireException(SpaceWireException::ReceiveFailed));
	}
	vector<unsigned char> packet;
	unsigned int size=(unsigned int)properties->len;
	for(unsigned int i=0;i<size;i++){
		packet.push_back(((unsigned char*)receivebuffer)[i]);
	}
	SpaceWire_FreeRead(spwhandler,*packetid);
	Sfree(properties);
	Sfree(packetid);
	SpaceWireUtilities::dumpPacket(&packet);	
	
	ext_tsk();
}

void sendtask(W arg){
	cout << "send" << endl;
	unsigned char p[]={0x01,0x01,0x4c,0x02,0xfe,0x99,0xaa,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x14};
	PVOID sendbuffer=Smalloc(1000);
	for(int i=0;i<16;i++){
		((unsigned char*)sendbuffer)[i]=p[i];
	}
	SPACEWIRE_STATUS status;
	SPACEWIRE_ID* packetid=(SPACEWIRE_ID*)Smalloc(sizeof(SPACEWIRE_ID));
	status=SpaceWire_SendPacket(spwhandler,sendbuffer,16,false,packetid);
	if(status!=TRANSFER_STARTED){
		throw(SpaceWireException(SpaceWireException::SendFailed));
	}
	Condition c;
	cout << "send completed" << endl;
	c.wait();
}
int main(int argc,char* argv[]){
	bool result=SpaceWire_Open(&spwhandler,0);
	result=SpaceWire_RegisterReceiveOnAllPorts(spwhandler);
	W receivetaskid=cre_tsk(receivetask,-1,0);
	Condition c;
	c.wait(2000);
	W sendtaskid=cre_tsk(sendtask,-1,0);
	cout << "main waiting" << endl;
	c.wait();
	return 0;
}