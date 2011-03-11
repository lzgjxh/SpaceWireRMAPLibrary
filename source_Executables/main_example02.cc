#include "SpaceWire.hh"
#include "RMAP.hh"

#include "Condition.hh"
#include "Mutex.hh"
#include "Thread.hh"

#include <exception>
#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
using namespace std;

class SingleTransaction : public Thread {
private:
	unsigned int id;
	Mutex* mutex;
	RMAPEngine* rmapengine;
	RMAPSocket* rmapsocket;
	RMAPDestination rmapdestination;
	unsigned int address;
	unsigned int length;
	static int activethreads;
public:
	SingleTransaction(unsigned int newid,Mutex* newmutex,RMAPEngine* newrmapengine,RMAPDestination newrmapdestination,unsigned int newaddress,unsigned int newlength)
	: Thread(),id(newid),mutex(newmutex),rmapengine(newrmapengine),rmapdestination(newrmapdestination),address(newaddress),length(newlength){}
	~SingleTransaction(){}
	void countup(){
		mutex->lock();
		activethreads++;
		mutex->unlock();
	}
	static int getActiveThreads(){
		return activethreads;
	}
	void run(){
		//uncomment out these lines if you use Shimafuji DIO Board as a target
		//rmapdestination.setDestinationKey(0x02);
		//rmapdestination.setDestinationLogicalAddress(0x01);
		
		rmapsocket=rmapengine->openRMAPSocketTo(rmapdestination);
		mutex->lock();
		cout << "Thread id:" << id << " Read (address=" << setw(8) << setfill('0') << hex << address;
		cout << " length=" << setw(2) << setfill('0') << length << ") activethreads=" << activethreads << endl; 
		mutex->unlock();
		vector<unsigned char>* data=rmapsocket->read(address,length);
		mutex->lock();
		cout << "Thread id:"<< id << " Completed (replied data length=" << setw(2) << setfill('0') << data->size() << ")" << endl;
		cout << "received data : ";
		SpaceWireUtilities::dumpPacket(data);
		activethreads--;
		mutex->unlock();
		rmapengine->closeRMAPSocket(rmapsocket);
		this->exit();
	}
};

int SingleTransaction::activethreads=0;

class Main {
private:
	SpaceWireIF* spacewireif;
	RMAPEngine* rmapengine;
	
	SpaceWireCLI cli;
	
	int menu; //selected command number
	
	unsigned int address;
	unsigned int length; //in a unit of byte
	vector<unsigned char> writedata;
	vector<unsigned char>* readdata;
	
	map<unsigned int,RMAPPacket*> packetmap;
	static const unsigned int ExpectedPacketNumber=8;
	
	Mutex mutex;
public:
	Main(){
		//initialize SpaceWire IF
		spacewireif=SpaceWireIFImplementations::selectInstanceFromCommandLineMenu();
		//open SpW IF
		spacewireif->initialize();
		spacewireif->open();
		//open RMAPEngine
		rmapengine=new RMAPEngine(spacewireif);
		rmapengine->start();
	}
	~Main(){
		rmapengine->stop();
		spacewireif->close();
		delete rmapengine;
		delete spacewireif;
	}
	void run(){
		//title
		showTitle();

		loop:

		//show menu
		showMenu();
		//selection
		cli.ask2_int(cin,menu,cout,"select > ");
		//process selected menu
		processCommand(menu);
	}

private:
	void showTitle() {
		cout << "############################################" << endl;
		cout << "## Multiple RMAP Transaction Test Program ##" << endl;
		cout << "############################################" << endl << endl;
	}

	void showMenu() {
		cout << "Menu : " << endl;
		cout << "  MT Test (Source Mode)     [1]" << endl;
		cout << "  MT Test (Destination Mode)[2]" << endl;
		cout << "  Quit                      [9]" << endl << endl;
	}
private:
	void processCommand(int menu) {
		switch (menu) {
		case 1:
			mtTestSource();
			break;
		case 2:
			mtTestDestination();
			break;
		case 9: //Quit
			break;
		}
	}
private:
	void mtTestSource(){
		cout << "Multiple Transaction Test (Source Mode)" << endl;
		SingleTransaction* st[ExpectedPacketNumber];
		Condition condition;
		
		for(unsigned int i=0;i<ExpectedPacketNumber;i++){
			RMAPDestination rmapdestination;
			rmapdestination.setDestinationKey(i);
			unsigned int address=i;
			unsigned int length=i+1;
			st[i]=new SingleTransaction(i,&mutex,rmapengine,rmapdestination,address,length);
			st[i]->countup();
			st[i]->start();
			condition.wait(100);
		}
		
		while(SingleTransaction::getActiveThreads()!=0){
			condition.wait(1000);
			cout << "The number of active SingleTransaction instances : " << SingleTransaction::getActiveThreads() << endl;
		}
		cout << endl;
		cout << "Completed" << endl << endl;
	}
	void mtTestDestination(){
		cout << "Multiple Transaction Test (Destination Mode)" << endl;
		rmapengine->suspend();
		
		unsigned int packetcount=0;
		while(packetcount!=ExpectedPacketNumber){
			cout << "Waiting a packet (" << dec << packetcount << "/" << dec << ExpectedPacketNumber << "received)...";
			vector<unsigned char> data=rmapengine->receive();
			if(data.size()!=0){
				RMAPPacket* packet=new RMAPPacket();
				try{
					packet->setPacket(&data);
					unsigned int tid=packet->getTransactionID();
					cout << "received (TID=" << hex << tid << ")" << endl;
					packetmap.insert(pair<unsigned int,RMAPPacket*>(tid,packet));
					packetcount++;
				}catch(RMAPException e){
					cout << "RMAPException : not an RMAP packet" << endl;
					SpaceWireUtilities::dumpPacket(&data);
				}catch(exception e){
					cout << "exception : not an RMAP packet" << endl;
					SpaceWireUtilities::dumpPacket(&data);
				}
			}
		}
		cout << endl;
		cout << "Start Replying" << endl;
		vector<unsigned int> repliedtid;
		while(packetmap.size()!=0){
			cout << "Remaining TIDs : ";
			map<unsigned int,RMAPPacket*>::iterator i=packetmap.begin();
			while(i!=packetmap.end()){
				cout << i->first << " ";
				i++;
			}
			cout << endl;
			int tid;
			tidloop:
			cli.ask2_int(cin,tid,cout,"select tid to be replied> ");
			if(packetmap.find(tid)!=packetmap.end()){
				RMAPPacket* pointer=(packetmap.find(tid))->second;
				RMAPPacket commandpacket=*(pointer);
				delete pointer;
				commandpacket.interpretPacket();
				RMAPPacket* replypacket=new RMAPPacket();
				RMAPDestination destination;
				destination.setDestinationKey(commandpacket.getRMAPDestination()->getDestinationKey());
				replypacket->setDestination(destination);
					
				replypacket->setReplyStatus(0x00); //successfull
				replypacket->setReplyPacket();
				replypacket->setWriteOrRead(commandpacket.getWriteOrRead());
				replypacket->setTransactionID(commandpacket.getTransactionID());
				replypacket->setVerifyMode(commandpacket.getVerifyMode());
				replypacket->setVerifyMode(commandpacket.getVerifyMode());
				replypacket->setAckMode(commandpacket.getAckMode());
				replypacket->setIncrementMode(commandpacket.getIncrementMode ());
				replypacket->setExtendedAddress(commandpacket.getExtendedAddress());
				replypacket->setAddress(commandpacket.getAddress());
				replypacket->setLength(commandpacket.getLength());
				if(commandpacket.isReadMode()){
					vector<unsigned char> data;
					for(int i=0;i<commandpacket.getLength();i++){
						data.push_back((unsigned char)(i%0x100));
					}
					replypacket->setData(&data);
				}
				replypacket->createPacket();
				cout << "replying...";
				rmapengine->send(replypacket->getPacket());
				cout << "done" << endl << endl;
				delete replypacket;
				packetmap.erase(tid);
			}else{
				goto tidloop;
			}
		}
		rmapengine->resume();
		cout << "Reply Completed" << endl << endl;
	}
};


int main(int argc,char* argv[]){
	Main main;
	main.run();
	return 0;
}

/** History
 * 2008-05-xx file created (Takayuki Yuasa)
 * 2008-09-04 use SpaceWireIFImplementations::selectInstanceFromCommandLineMenu() (Takayuki Yuasa)
 */
