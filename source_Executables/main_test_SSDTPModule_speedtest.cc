#include "SpaceWireIFViaSpW2TCPIPBridge.hh"
#include "SpaceWireCLI.hh"

#include "Thread.hh"
#include "Condition.hh"

#include "IPServerSocket.hh"
#include "IPClientSocket.hh"

#include <iostream>
#ifdef SYSTEM_TYPE_POSIX
#include <sys/time.h>
#include <time.h>
#endif
#ifdef SYSTEM_TYPE_T_KERNEL
#include "btron/tkcall.h"
#endif

#include <stdlib.h>
#include <string.h>
using namespace std;

Condition c;

class NodeServer : public Thread {
public:
	NodeServer() : Thread() {
		cout << "NodeServer instantiated" << endl;
	}
	~NodeServer(){}
	void run(){
		IPServerSocket datasocket_listen(SpaceWireIFViaSpW2TCPIPBridge::spw2tcpiphongo_dataport);
		
		datasocket_listen.open();
		
		IPServerAcceptedSocket* datasocket;
		try{
			datasocket=datasocket_listen.accept();
		}catch(IPSocketException e){
			cout << "NodeServer accept() exception" << endl;
			::exit(-1);
		}
		
		SSDTPModule* ssdtpmodule=new SSDTPModule(datasocket);
		
		//first receive iteration number
		vector<unsigned char> tmp=ssdtpmodule->receive();
		int iteration=tmp.at(0)*0x0100+tmp.at(1);
		
		cout << "Transffering " << iteration << "times (tmp.size()=" << tmp.size() << ")" << endl;cout.flush();
		int count=0;
		while(count<iteration){
			vector<unsigned char> data=ssdtpmodule->receive();
			count++;
		}
		//reply done(toriaezu data ha nandemo yoi)
		ssdtpmodule->send(tmp);
		cout << "Completed" << endl;
		cout << endl;
		
		datasocket->close();

		datasocket_listen.close();
		delete ssdtpmodule;
		
		c.wait(2000);
		c.signal();
	}
};

class NodeClient : public Thread {
private:
	SpaceWireCLI cli;
	string hostname;
	unsigned int address;
	unsigned int length; //in a unit of byte
	unsigned int iteration;
	unsigned int displayfrequency;
#ifdef SYSTEM_TYPE_POSIX
	struct timeval starttime,endtime,duration;
#endif

public:
	NodeClient() : Thread() {
		cout << "NodeClient instantiated" << endl;
	}
	~NodeClient(){}
	void setHostname(string newhostname){
		hostname=newhostname;
	}
	void run(){
		setParameters();
		
		IPClientSocket datasocket(hostname,SpaceWireIFViaSpW2TCPIPBridge::spw2tcpiphongo_dataport);
		try{
			cout << "connecting to " << hostname << endl;
			this->sleep(200);
			datasocket.open();
		}catch(IPSocketException e){
			cout << "couldnot connect to " << hostname << endl;
			::exit(-1);
		}
		
		SSDTPModule* ssdtpmodule=new SSDTPModule(&datasocket);
		
		//first tell iteration number
		vector<unsigned char>* data=new vector<unsigned char>();
		data->push_back((unsigned char)(iteration/0x0100));
		data->push_back((unsigned char)(iteration%0x0100));
		ssdtpmodule->send(*data);
		data->clear();
		
		cout << "NodeClient is sending " << length << "byte data " << iteration << "times" << endl;cout.flush();
		
		for(unsigned int i=0;i<length;i++){
			data->push_back((unsigned char)i);
		}

#ifdef SYSTEM_TYPE_POSIX
		gettimeofday(&starttime,NULL);
#endif
#ifdef SYSTEM_TYPE_T_KERNEL
		SYSTIM pk_tim_start;
		SYSTIM pk_tim_end;
		ER ercd;
		ercd=tk_get_tim(&pk_tim_start);
#endif
		
		unsigned int count=0;
		while(count<iteration){
			ssdtpmodule->send(*data);
			count++;
			if(count%displayfrequency==0){
				cout << "Done (" << count+1 << ")" << endl;
			}
		}
		
		vector<unsigned char> done=ssdtpmodule->receive();
		
		double millis=0;

#ifdef SYSTEM_TYPE_POSIX
		gettimeofday(&endtime,NULL);
		timersub(&endtime,&starttime,&duration);
		millis=duration.tv_sec*1000+duration.tv_usec/1000.;
#endif
#ifdef SYSTEM_TYPE_T_KERNEL
		ercd=tk_get_tim(&pk_tim_end);
		W dhi=pk_tim_start.hi-pk_tim_end.hi;
		UW dlow=pk_tim_end.lo-pk_tim_start.lo;
		millis=dlow;
#endif
		
		cout << "Completed " << (double)(length*iteration)/1000. << "kBytes in " << dec << millis/1000 << "sec" << endl;
		cout << "Bit-rate    " << (double)(length*iteration)/1000./(millis/1000)*8 << "kbit/s" << endl;

		cout << "Completed" << endl << endl;
		datasocket.close();
		delete ssdtpmodule;
		
		c.signal();
	}
private:
	void setParameters(){
		cout << "length of each access" << endl;
		length=askLength();
		cout << "number of iteration" << endl;
		iteration=askIteration();
		cout << "display frequency" << endl;
		displayfrequency=askDisplayFrequency();
	}
	unsigned int askLength(){
		int value;
		asklengthloop:
		cli.ask2_int(cin,value,cout,"length in decimal > ");
		if(value<0){
			goto asklengthloop;
		}else{
			return value;
		}
	}	
	unsigned int askIteration(){
		int value;
		askiterationloop:
		cli.ask2_int(cin,value,cout,"iteration number in decimal > ");
		if(value<0 || 0xFFFF<value){
			goto askiterationloop;
		}else{
			return value;
		}
	}	
	unsigned int askDisplayFrequency(){
		int value;
		askfrequencyloop:
		cli.ask2_int(cin,value,cout,"show degree of process per N accesses : N > ");
		if(value<0){
			goto askfrequencyloop;
		}else{
			return value;
		}
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
