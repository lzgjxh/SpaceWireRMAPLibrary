#include "SpaceWireIFViaSpW2TCPIPBridge.hh"
#include "Thread.hh"
#include "Condition.hh"

#include "IPServerSocket.hh"
#include "IPClientSocket.hh"

#include <iostream>
using namespace std;

Condition c;

class NodeServer : public Thread {
public:
	NodeServer() : Thread() {
		cout << "NodeServer instantiated" << endl;
	}
	void run(){
		IPServerSocket datasocket_listen(SpaceWireIFViaSpW2TCPIPBridge::spw2tcpiphongo_send_dataport);
		IPServerSocket controlsocket_listen(SpaceWireIFViaSpW2TCPIPBridge::spw2tcpiphongo_send_controlport);
		
		datasocket_listen.open();
		controlsocket_listen.open();
		
		IPServerAcceptedSocket datasocket;
		IPServerAcceptedSocket controlsocket;
		try{
			datasocket=datasocket_listen.accept();
			controlsocket=controlsocket_listen.accept();
		}catch(IPSocketException e){
			cout << "NodeServer accept() exception" << endl;
			::exit(-1);
		}
		
		SSDTPModule* ssdtpmodule=new SSDTPModule(&datasocket,&controlsocket);
		
		cout << "NodeServer is waiting for data" << endl;cout.flush();
		vector<unsigned char> data=ssdtpmodule->receive();
		cout << "NodeServer data received" << endl;
		for(int i=0;i<data.size();i++){
			cout << " " << (unsigned int)data.at(i);
		}
		cout << endl;
		
		datasocket.close();
		controlsocket.close();
		
		datasocket_listen.close();
		controlsocket_listen.close();
		delete ssdtpmodule;
		
		c.wait(2000);
		c.signal();
	}
};

class NodeClient : public Thread {
private:
	string hostname;
public:
	NodeClient() : Thread() {
		cout << "NodeClient instantiated" << endl;
	}
	void setHostname(string newhostname){
		hostname=newhostname;
	}
	void run(){
		IPClientSocket datasocket(hostname,SpaceWireIFViaSpW2TCPIPBridge::spw2tcpiphongo_send_dataport);
		IPClientSocket controlsocket(hostname,SpaceWireIFViaSpW2TCPIPBridge::spw2tcpiphongo_send_controlport);
		try{
			this->sleep(200);
			datasocket.open();
			this->sleep(200);
			controlsocket.open();
		}catch(IPSocketException e){
			cout << "couldnot connect to " << hostname << endl;
			::exit(-1);
		}
		
		SSDTPModule* ssdtpmodule=new SSDTPModule(&datasocket,&controlsocket);
		cout << "NodeClient is sending 16 byte data" << endl;cout.flush();
		vector<unsigned char>* data=new vector<unsigned char>();
		for(unsigned char i=0;i<16;i++){
			cout << " " << (int)i;
			data->push_back(i);
		}
		cout << endl;
		
		ssdtpmodule->send(*data);
		
		datasocket.close();
		controlsocket.close();
		delete ssdtpmodule;
		
		c.wait(2000);
		c.signal();
	}
};

int main(int argc,char* argv[]){
	cout << "SSDTPModule Test Program" << endl;
	cout << "Takayuki Yuasa et al." << endl << endl;
	cout << "select mode : " << endl;
	cout << " 1 : server" << endl;
	cout << " 2 : client" << endl;
	//cout << " 3 : domestic (server=client=localhost)" << endl;
	cout << " > ";
	string str;
	int menu=0;
	
	selection:
	cin >> str;
	menu=atoi(str.c_str());
	if(menu!=1 && menu!=2 && menu!=3){
		goto selection;
	}
	
	NodeServer* server;
	NodeClient* client;
	if(menu==1){
		server=new NodeServer();
		server->start();
	}else if(menu==2){
		string hostname;
		cout << "input server hostname : ";
		cin >> hostname;
		cout << endl;
		client=new NodeClient();
		client->setHostname(hostname);
		client->start();
	}else if(menu==3){
		server=new NodeServer();
		server->start();
		client=new NodeClient();
		client->setHostname("localhost");
		client->start();
	}
	c.wait(); //c is an instance of Condition class that is declared globally (see the top of this file)
	cout << "quitting..." << endl;
	if(menu==1){
		delete server;
	}else{
		delete client;
	}
	
}