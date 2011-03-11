#include "IPServerSocket.hh"

#include <iostream>
#include <stdlib.h>
using namespace std;

int main(int argc,char* argv[]){
	cout << "----------------------------------------" << endl;
	cout << "Test program for IPServerSocket class (Takayuki Yuasa)." << endl;
	cout << "This program waits for a client connection at port 10024" << endl;
	cout << "as 'echo-server'. To check the functionality, please" << endl;
	cout << "connect to this server with telnet, and then type some keys." << endl;
	cout << "Example : in other terminal emulator" << endl;
	cout << " bash > telnet localhost 10024" << endl;
	cout << "To quit, press Ctrl-C" << endl;
	cout << "----------------------------------------" << endl;
	
	IPServerSocket server(10024);
	server.open();
	
	IPServerAcceptedSocket* acceptedsocket;
	
	cout << "Waiting for a connection at port 10024...";cout.flush();
	try{
		acceptedsocket=server.accept();
	}catch(IPSocketException e){
		cout << "could not accept a client" << endl;
	}
	cout << "Connected" << endl;
	
	unsigned char data[1000];
	int receivedsize;
	
	do{
		cout << "Waiting data..." << endl; 
		try{
			receivedsize=acceptedsocket->receive(data,100);
		}catch(IPSocketException e){
			cout << "exception in receiving" << endl;
			exit(-1);
		}
		if(receivedsize!=0){
			data[receivedsize]=0;
			cout << "Received : " << data << endl;
			cout << "Echo back it...";
			try{
				acceptedsocket->send(data,receivedsize);
			}catch(IPSocketException e){
				cout << "exception in sending" << endl;
				exit(-1);
			}
			cout << "done" << endl;
		}
	}while(receivedsize!=0);
}
