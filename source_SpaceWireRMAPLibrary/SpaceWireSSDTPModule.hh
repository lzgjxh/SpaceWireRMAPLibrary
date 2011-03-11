#ifndef SPACEWIRESSDTPMODULE_HH_
#define SPACEWIRESSDTPMODULE_HH_

#include "IPServerSocket.hh"
#include "IPClientSocket.hh"

#include "SpaceWireIF.hh"
#include "Mutex.hh"
#include "Condition.hh"

#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
using namespace std;

/** Exception class for SSDTPModule.
 */
class SSDTPException{
private:
	string status;
public:
	SSDTPException(){}
	SSDTPException(string str){status=str;}
	string getStatus(){
		return status;
	}
	void dump(){
		cout << status << endl;
	}
};

/** This class performs synchronous data transfer via
 * TCP/IP network using "Simple- Synchronous- Data Transfer Protocol"
 * which is defined for this class.
 */
class SSDTPModule {
public:
	static const unsigned int BufferSize=1024*1024;
private:
	IPSocket* datasocket;
	unsigned char* sendbuffer;
	unsigned char* receivebuffer;
	stringstream ss;
	unsigned char internal_timecode;
	unsigned int latest_sentsize;
	Mutex sendmutex;
	Mutex receivemutex;
	SpaceWireTimeCodeAction* timecodeaction;
public:
	SSDTPModule(IPSocket* newdatasocket);
	~SSDTPModule();
	void send(vector<unsigned char>& data) throw(SSDTPException);
	void send(vector<unsigned char>* data) throw(SSDTPException);
	void send(unsigned char* data, unsigned int size) throw(SSDTPException);
	vector<unsigned char> receive() throw(SSDTPException);
	int receive(vector<unsigned char>* data) throw(SSDTPException);
	void sendEEP() throw(SSDTPException);
	void sendTimeCode(unsigned char timecode) throw(SSDTPException);
	unsigned char getTimeCode() throw(SSDTPException);
	void setTimeCodeAction(SpaceWireTimeCodeAction* action);
	void setTxFrequency(double frequencyInMHz);
private:
	void gotTimeCode(unsigned char timecode);
	void registerRead(unsigned int address);
	void registerWrite(unsigned int address,vector<unsigned char> data);
public:
	void setTxDivCount(unsigned char txdivcount);
public:
	/* for SSDTP2 */
	static const unsigned char DataFlag_Complete_EOP=0x00;
	static const unsigned char DataFlag_Complete_EEP=0x01;
	static const unsigned char DataFlag_Flagmented=0x02;
	static const unsigned char ControlFlag_SendTimeCode=0x30;
	static const unsigned char ControlFlag_GotTimeCode=0x31;
	static const unsigned char ControlFlag_ChangeTxSpeed=0x38;
	static const unsigned char ControlFlag_RegisterAccess_ReadCommand=0x40;
	static const unsigned char ControlFlag_RegisterAccess_ReadReply=0x41;
	static const unsigned char ControlFlag_RegisterAccess_WriteCommand=0x50;
	static const unsigned char ControlFlag_RegisterAccess_WriteReply=0x51;
	static const unsigned int LengthOfSizePart=10;
private:
	/* for SSDTP2 */
	Mutex registermutex;
	Condition registercondition;
	map<unsigned int,unsigned int> registers;
public:
	/* for SSDTP1 */
	static const unsigned char ControlFlag=0x01;
	static const unsigned char DataFlag=0x00;
	static const unsigned char Null=0xFF;
	static const unsigned int TIMECODE_FOOTER_SIZE=15;
	static const unsigned char EOP=0x01;
	static const unsigned char EEP=0x02;
	static const unsigned char Timecode=0x03;
	static const unsigned char Quit=0x99;

private:
	unsigned char rheader[12];
	unsigned char r_tmp[30];
	unsigned char sheader[12];
public:
	int receivedsize;
	int rbuf_index;

};

#endif /*SPACEWIRESSDTPMODULE_HH_*/

/** History
 * 2008-06-xx file created (Takayuki Yuasa)
 * 2008-12-17 TimeCode implemented (Takayuki Yuasa)
 */
