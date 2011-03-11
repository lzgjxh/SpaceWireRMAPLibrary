#include "IPClientSocket.hh"

#include <iostream>
#include <stdlib.h>
using namespace std;

int main(int argc,char* argv[]){
	cout << "----------------------------------------" << endl;
	cout << "Test program for IPClientSocket class (Takayuki Yuasa)." << endl;
	cout << "This program connects to the given hostname with a port number of 80 (HTTP)," << endl;
	cout << "and then, send 'GET /' to the connected host to retrieve index page." << endl;
	cout << "----------------------------------------" << endl;
	
	string hostname;
	cout << "Input hostname to be connected (e.g. www.google.com) : ";
	cin >> hostname;
	cout << endl;
	
	IPClientSocket client(hostname.c_str(),80);
	
	cout << "Connecting to " << hostname << endl;
	try{
		client.open();
	}catch(IPSocketException e){
		cout << "could not connect to " << hostname << endl;
		cout << "Quitting" << endl;
		exit(-1);
	}
	
	cout << "Retrieving index" << endl;
	cout << "----------------------------------------" << endl;
	unsigned char data[10000];
	string get="GET /\r\n";
	client.send((void*)get.c_str(),get.size());
	
	int receivedsize;
	do{
		receivedsize=client.receive(data,100);
		data[receivedsize]=0;
		cout << data << endl;
	}while(receivedsize!=0);

	cout << "----------------------------------------" << endl;
	cout << "Closing socket" << endl;
	client.close();
	
	cout << "Quitting" << endl;
}
