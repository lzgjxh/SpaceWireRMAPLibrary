//SpaceWire to TCP/IP bridge Hongo
//main_sthongo.cc

#include "SpaceWire.hh"
#include "RMAP.hh"

#include "IPServerSocket.hh"

#include "Thread.hh"
#include "Mutex.hh"

#include <iostream>
using namespace std;

Mutex mutex;
Condition condition;
bool RunState;

class SpaceWire2TCPIPBridge_SendThread : public Thread {
private:
	SpaceWireIF* spacewireif;
	SSDTPModule* ssdtp;
public:
	SpaceWire2TCPIPBridge_SendThread(SpaceWireIF* spacewireif,SSDTPModule* ssdtp) : Thread() {
		this->spacewireif=spacewireif;
		this->ssdtp=ssdtp;
	}
	~SpaceWire2TCPIPBridge_SendThread(){
	}
	void run(){
		mutex.lock();
		cout << "running..." << endl;
		mutex.unlock();

		try{
			vector<unsigned char>* packet=new vector<unsigned char>;
			sendthread_while_loop:
			while(RunState){
#ifdef DEBUG
				mutex.lock();
				cout << "waiting sent-packet" << endl;
				mutex.unlock();
#endif

				//receive packet from TCP/IP
				try{
					if(ssdtp->receive(packet)<0){
						spacewireif->sendTimeCode(ssdtp->getTimeCode());
						goto sendthread_while_loop;
					}
				}catch(SSDTPException e){
					if(e.getStatus()=="TimeCode"){
						try{
							//cout << "Sending TimeCode " << dec << (unsigned int)ssdtp->getTimeCode() << endl;
							spacewireif->sendTimeCode(ssdtp->getTimeCode());
						}catch(SpaceWireException e){
							cout << "SpaceWireException in SendThread Send TimeCode" << endl;
						}
						goto sendthread_while_loop;
					}else{
						cout << "SendThread : IP connection lost" << endl;
						RunState=false;
						goto sendthread_while_loop;
					}
				}catch(...){
					cout << "SendThread : IP connection lost" << endl;
					RunState=false;
					goto sendthread_while_loop;
				}
#ifdef DEBUG
				mutex.lock();
				for(int i=0;i<packet->size();i++){
					cout << " " << setw(2) << setfill('0') << hex << (unsigned int)packet->at(i);
				}
				cout << "sending received TCP/IP packet to SpaceWire" << endl;
				mutex.unlock();
#endif

				//send the received packet to SpaceWire IF
				try{
					if(packet!=0){
						spacewireif->send(packet);
					}
				}catch(SpaceWireException e){
					cout << "SpaceWireException in SendThread" << endl;
				}
			}
		}catch(exception e){
			cout << "SpaceWire2TCPIPBridge_SendThread Quitting" << endl;
		}catch(...){
			cout << "exception in SendThread" << endl;
		}
	}
};

class SpaceWire2TCPIPBridge_ReceiveThread : public Thread {
private:
	SpaceWireIF* spacewireif;
	SSDTPModule* ssdtp;
public:
	SpaceWire2TCPIPBridge_ReceiveThread(SpaceWireIF* spacewireif,SSDTPModule* ssdtp) : Thread() {
		this->spacewireif=spacewireif;
		this->ssdtp=ssdtp;
	}
	~SpaceWire2TCPIPBridge_ReceiveThread(){
	}
	void run_old(){
		mutex.lock();
		cout << "running..." << endl;
		mutex.unlock();
		try{
			vector<unsigned char>* packet=new vector<unsigned char>;
			receivethread_while_loop:
			while(RunState){
#ifdef DEBUG
				mutex.lock();
				cout << "waiting received-packet" << endl;
				mutex.unlock();
#endif
				//receive packet from SpaceWire IF
				try{
					spacewireif->receive(packet);
				}catch(SpaceWireException e){
					//cout << "SpaceWireException in ReceiveThread" << endl;
					goto receivethread_while_loop;
				}
#ifdef DEBUG
				mutex.lock();
				for(int i=0;i<packet->size();i++){
					cout << " " << setw(2) << setfill('0') << hex << (unsigned int)packet->at(i);
				}
				cout << endl;
				mutex.unlock();
#endif
				if(packet->size()!=0){
#ifdef DEBUG
					mutex.lock();
					cout << "sending received SpaceWire packet to TCP/IP" << endl;
					mutex.unlock();
#endif

					//send the received packet to TCP/IP
					try{
						ssdtp->send(packet);
					}catch(...){
						cout << "ReceiveThread : IP connection lost" << endl;
						RunState=false;
					}
				}
			}
		}catch(exception e){
			cout << "SpaceWire2TCPIPBridge_ReceiveThread Quitting" << endl;
		}catch(...){
			cout << "exception in ReceiveThread" << endl;
		}
	}
	void run(){
		mutex.lock();
		cout << "running..." << endl;
		mutex.unlock();
		try{
			unsigned char* packet;
			packet=(unsigned char*)malloc(100000);
			unsigned int size=0;
			receivethread_while_loop:
			while(RunState){
#ifdef DEBUG
				mutex.lock();
				cout << "waiting received-packet" << endl;
				mutex.unlock();
#endif
				//receive packet from SpaceWire IF
				try{
					spacewireif->receive(packet,&size);
				}catch(SpaceWireException e){
					//cout << "SpaceWireException in ReceiveThread" << endl;
					goto receivethread_while_loop;
				}
				if(size!=0){
#ifdef DEBUG
					mutex.lock();
					cout << "sending received SpaceWire packet to TCP/IP" << endl;
					mutex.unlock();
#endif

					//send the received packet to TCP/IP
					try{
						ssdtp->send(packet,size);
					}catch(...){
						cout << "ReceiveThread : IP connection lost" << endl;
						RunState=false;
					}
				}
			}
		}catch(exception e){
			cout << "SpaceWire2TCPIPBridge_ReceiveThread Quitting" << endl;
		}catch(...){
			cout << "exception in ReceiveThread" << endl;
		}
	}

};

class SpaceWire2TCPIPBridge {
private:
	SpaceWireIF* spacewireif;
	SpaceWireCLI cli;
	static const int timeoutduration=100; //100ms timeout
public:
	void run(int argc,char* argv[]){
		cout << "====================================" << endl;
		cout << "SpaceWire to TCP/IP bridge : sthongo" << endl;
		cout << "  Takayuki Yuasa et al. June 2008   " << endl;
		cout << endl;
		cout << "  This software is supported by     " << endl;
		cout << "  SpaceWire/RMAP 'Hongo' Library.   " << endl;
		cout << " date : 2010-03-23" << endl;
		cout << "====================================" << endl;
		cout << endl;


		//open SpaceWire IF
		cout << "opening SpaceWire IF..." << endl;

		spacewireif=NULL;
		if(argc>1){
			string spacewireifname(argv[1]);
			cout << "SpaceWire Interface " << spacewireifname << endl;
			try{
				spacewireif=SpaceWireIFImplementations::createInstanceFromName(spacewireifname);
			}catch(SpaceWireException e){
				e.dump();
			}
		}
		if(spacewireif==NULL){
			spacewireif=SpaceWireIFImplementations::selectInstanceFromCommandLineMenu();
		}

		spacewireif->initialize();
		spacewireif->open();
		SpaceWireLinkStatus linkstatus=spacewireif->getLinkStatus();
		linkstatus.setTimeout(100);
		spacewireif->setLinkStatus(linkstatus);

		//open listening server socket
		unsigned int tcpipportnumber=SpaceWireIFOverIPServer::spw2tcpiphongo_dataport;
		if(argc>2){
			//argv[2] : TCP/IP port number
			tcpipportnumber=atoi(argv[2]);
		}
		IPServerSocket datasocket_listen(tcpipportnumber);
		try{
			datasocket_listen.open();
		}catch(IPSocketException e){
			cout << "SpaceWire2TCPIPBridge::run() sockets could not be opened..." << endl;
			::exit(-1);
		}

		//loop
		loop:
		RunState=true;
		cout << "waiting for a new connection from client..." << endl;
		IPServerAcceptedSocket* datasocket=datasocket_listen.accept();
		SSDTPModule* ssdtp=new SSDTPModule(datasocket);
		SpaceWire2TCPIPBridge_SendThread* sendthread=new SpaceWire2TCPIPBridge_SendThread(spacewireif,ssdtp);
		SpaceWire2TCPIPBridge_ReceiveThread* receivethread=new SpaceWire2TCPIPBridge_ReceiveThread(spacewireif,ssdtp);

		cout << "starting daemons..." << endl;
		sendthread->start();
		receivethread->start();

		sendthread->join();
		cout << "SendThread joined" << endl;
		cout << "Quitting ReceiveThread" << endl;
		RunState=false; //stop receivethread
		condition.wait(timeoutduration*2);

		try{
			cout << "closing socket" << endl;
			datasocket->close();
			delete datasocket;

			cout << "deleting threads" << endl;
			delete sendthread;
			delete receivethread;
			delete ssdtp;

		}catch(...){
			cout << "exception was thrown when deleting instances" << endl;
			::exit(-1);
		}

		cout << endl << endl;
		goto loop;
	}
};

int main(int argc,char* argv[]){
	SpaceWire2TCPIPBridge stbridge;
	try{
		stbridge.run(argc,argv);
	}catch(exception e){
		cout << "main() Quitting" << endl;
	}
}

/** History
 * 2008-07-xx file created (Takayuki Yuasa)
 * 2008-10-30 tuning to improve transfer speed
 *            wait for reconnection when disconnected (Takayuki Yuasa)
 *
 */
