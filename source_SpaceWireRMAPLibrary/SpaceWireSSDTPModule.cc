#include "SpaceWireSSDTPModule.hh"

#include <stdlib.h>
#include <string.h>
using namespace std;

#define SSDTP2
#undef SSDTP1

SSDTPModule::SSDTPModule(IPSocket* newdatasocket){
	datasocket=newdatasocket;
	sendbuffer=(unsigned char*)malloc(SSDTPModule::BufferSize*2);
	receivebuffer=(unsigned char*)malloc(SSDTPModule::BufferSize*2);
	internal_timecode=0x00;
	latest_sentsize=0;
	timecodeaction=NULL;
#ifdef SSDTP1
	cout << "SSDTPModule : SSDTP1 Protocol is used." << endl;
	receivedsize=0;
	rbuf_index=0;
	//cout << "#0_0 receivedsize:" << receivedsize << " rbuf_index:" << rbuf_index << endl;
#endif
#ifdef SSDTP2
	cout << "SSDTPModule : SSDTP2 Protocol is used." << endl;
#endif
}

SSDTPModule::~SSDTPModule(){
	free(sendbuffer);
}

void SSDTPModule::send(vector<unsigned char>& data) throw(SSDTPException) {
	sendmutex.lock();
	send(&data);
	sendmutex.unlock();
}

void SSDTPModule::send(vector<unsigned char>* data) throw(SSDTPException){
#ifdef SSDTP2
	sendmutex.lock();
	unsigned int size=data->size();
	sheader[0]=0x00;
	sheader[1]=0x00;
	for(unsigned int i=11;i>1;i--){
		sheader[i]=size%0x100;
		//cout << "setSize:sheader[" << i << "] "<< (unsigned int)sheader[i] << endl;
		size=size/0x100;
	}
	try{
		datasocket->send(sheader,12);
		datasocket->send(&(data->at(0)),data->size());
	}catch(...){
		sendmutex.unlock();
		throw SSDTPException(SSDTPException::Disconnected);
	}
	sendmutex.unlock();
#endif

#ifdef SSDTP1
	sendmutex.lock();
	if(data->size()>SSDTPModule::BufferSize){
		throw SSDTPException(SSDTPException::DataSizeTooLarge);
	}
	int index=0;

	//data flag
	sendbuffer[index]=SSDTPModule::DataFlag;
	index++;
	ss.str("");
	ss << setw(SSDTPModule::LengthOfSizePart) << setfill('0') << data->size();
	strcpy((char*)(sendbuffer+index),(const char*)ss.str().c_str());
	index+=SSDTPModule::LengthOfSizePart;

	//send
	memcpy(sendbuffer+index,(void*)&(data->at(0)),data->size());
	index+=data->size();

	int ssize=index;
	int sentsize=0;
	while(sentsize<ssize){
		try{
			sentsize+=datasocket->send((void*)(sendbuffer+sentsize),ssize-sentsize);
		}catch(...){
			throw SSDTPException(SSDTPException::Disconnected);
		}
	}
	sendmutex.unlock();
#endif

}

void SSDTPModule::send(unsigned char* data, unsigned int size) throw(SSDTPException){
#ifdef SSDTP2
	sendmutex.lock();
	sheader[0]=0x00;
	sheader[1]=0x00;
	unsigned int asize=size;
	for(unsigned int i=11;i>1;i--){
		sheader[i]=asize%0x100;
		asize=asize/0x100;
	}
	try{
		datasocket->send(sheader,12);
		datasocket->send(data,size);
	}catch(...){
		sendmutex.unlock();
		throw SSDTPException(SSDTPException::Disconnected);
	}
	sendmutex.unlock();
#endif

#ifdef SSDTP1
	sendmutex.lock();
	if(size>SSDTPModule::BufferSize){
		throw SSDTPException(SSDTPException::DataSizeTooLarge);
	}
	int index=0;

	//data flag
	sendbuffer[index]=SSDTPModule::DataFlag;
	index++;
	ss.str("");
	ss << setw(SSDTPModule::LengthOfSizePart) << setfill('0') << size;
	strcpy((char*)(sendbuffer+index),(const char*)ss.str().c_str());
	index+=SSDTPModule::LengthOfSizePart;

	//send
	memcpy(sendbuffer+index,data,size);
	index+=size;

	int ssize=index;
	int sentsize=0;
	while(sentsize<ssize){
		try{
			sentsize+=datasocket->send((void*)(sendbuffer+sentsize),ssize-sentsize);
		}catch(...){
			throw SSDTPException(SSDTPException::Disconnected);
		}
	}
	sendmutex.unlock();
	/*
	if(size!=latest_sentsize){
		latest_sentsize=size;
		ss.str("");
		ss << setw(SSDTPModule::LengthOfSizePart) << setfill('0') << size;
		strcpy((char*)(sendbuffer+index),(const char*)ss.str().c_str());
	}
	index+=SSDTPModule::LengthOfSizePart;

	int ssize=index;
	int sentsize=0;
	while(sentsize<ssize){
		try{
			sentsize+=datasocket->send((void*)(sendbuffer+sentsize),ssize-sentsize);
		}catch(...){
			throw SSDTPException("SSDTPModule::send() exception");
		}
	}

	//send
	ssize=latest_sentsize;
	sentsize=0;
	while(sentsize<ssize){
		try{
			sentsize+=datasocket->send((void*)(data+sentsize),ssize-sentsize);
		}catch(...){
			throw SSDTPException("SSDTPModule::send() exception");
		}
	}*/
#endif
}


vector<unsigned char> SSDTPModule::receive() throw(SSDTPException) {
	receivemutex.lock();
	vector<unsigned char> data;
	receive(&data);
	receivemutex.unlock();
	return data;
}

int SSDTPModule::receive(vector<unsigned char>* data) throw(SSDTPException){
#ifdef SSDTP2

	receivemutex.lock();
	unsigned int size=0;
	unsigned int hsize=0;
	unsigned int flagment_size=0;
	unsigned int received_size=0;

	//header
	receive_header:
	rheader[0]=0xFF;
	rheader[1]=0x00;
	while(rheader[0]!=DataFlag_Complete_EOP && rheader[0]!=DataFlag_Complete_EEP){
		hsize=0;
		flagment_size=0;
		received_size=0;
		//flag and size part
		try{
			while(hsize!=12){
				//cerr << "receive header " << hsize << endl;cout.flush();
				int result=datasocket->receive(rheader+hsize,12-hsize);
				hsize+=result;
			}
		}catch(IPSocketException e){
			receivemutex.unlock();
			if(e.getStatus()==IPSocketException::Timeout){
				throw SSDTPException(SSDTPException::Timeout);
			}else{
				throw SSDTPException(SSDTPException::Disconnected);
			}
		}catch(...){
			receivemutex.unlock();
			throw SSDTPException(SSDTPException::Disconnected);
		}

		//data or control code part
		if(rheader[0]==DataFlag_Complete_EOP || rheader[0]==DataFlag_Complete_EEP || rheader[0]==DataFlag_Flagmented){
			//data
			for(unsigned int i=2;i<12;i++){
				flagment_size=flagment_size*0x100+rheader[i];
			}
			//unsigned char* data_pointer=&(data->at(0));
			unsigned char* data_pointer=receivebuffer;
			while(received_size!=flagment_size){
				int result;
				try {
					result =
							datasocket->receive(data_pointer + size + received_size, flagment_size - received_size);
				} catch (IPSocketException e) {
					cout << "SSDTPModule::receive() exception when receiving data" << endl;
					e.dump();
					cout << "rheader[0]=0x" << setw(2) << setfill('0') << hex << (unsigned int) rheader[0] << endl;
					cout << "rheader[1]=0x" << setw(2) << setfill('0') << hex << (unsigned int) rheader[1] << endl;
					cout << "size=" << dec << size << endl;
					cout << "flagment_size=" << dec << flagment_size << endl;
					cout << "received_size=" << dec << received_size << endl;
				}
				received_size+=result;
			}
			size+=received_size;
		}else if(rheader[0]==ControlFlag_SendTimeCode || rheader[0]==ControlFlag_GotTimeCode){
			//control
			unsigned char timecode_and_reserved[2];
			unsigned int tmp_size=0;
			try{
				while(tmp_size!=2){
					int result=datasocket->receive(timecode_and_reserved+tmp_size,2-tmp_size);
					tmp_size+=result;
				}
			}catch(...){
				receivemutex.unlock();
				throw SSDTPException(SSDTPException::TCPSocketError);
			}
			switch(rheader[0]){
			case ControlFlag_SendTimeCode:
				internal_timecode=timecode_and_reserved[0];
				receivemutex.unlock();
				throw SSDTPException(SSDTPException::TimecodeReceived);
				break;
			case ControlFlag_GotTimeCode:
				internal_timecode=timecode_and_reserved[0];
				receivemutex.unlock();
				gotTimeCode(internal_timecode);
				break;
			}
		}else{
			cout << "SSDTP fatal error with flag value of 0x" << hex << (unsigned int)rheader[0] << dec << endl;
			exit(-1);
		}
	}
	data->resize(size);
	if(size!=0){
		memcpy(&(data->at(0)),receivebuffer,size);
	}else{
		goto receive_header;
	}
	if(rheader[0]==DataFlag_Complete_EEP){
		receivemutex.unlock();
		throw SSDTPException(SSDTPException::EEP);
	}
	receivemutex.unlock();
#endif


#ifdef SSDTP2_OLD
	//header
	data->resize(1024*1024);
	rheader[0]=0x01;
	rheader[1]=0x00;
	unsigned int size=0;
	unsigned int hsize=0;
	unsigned int flagment_size=0;
	unsigned int received_size=0;
	while(rheader[0]!=0x00){
		hsize=0;
		flagment_size=0;
		received_size=0;
		//cout << "wait receive..." << endl;
		try{
			while(hsize!=2){
				//cerr << "receive header " << hsize << endl;cout.flush();
				int result=datasocket->receive(rheader+hsize,2-hsize);
				hsize+=result;
			}
		}catch(...){
			throw SSDTPException(SSDTPException::TCPSocketError);
		}

		if(rheader[0]==ControlFlag){
			//control code
			/*
			cout << "control code " << endl;cout.flush();
			cout << hex << setw(2) << setfill('0') << (unsigned int)rheader[0]<< endl;
			cout << hex << setw(2) << setfill('0') << (unsigned int)rheader[1]<< dec << endl;
			*/
			unsigned char controlcode[2];
			unsigned int controlcode_size=0;
			while(controlcode_size!=2){
				int result=datasocket->receive(controlcode,2-controlcode_size);
				controlcode_size+=result;
			}
			/*
			cout << "control code 2" << endl;
			cout << hex << setw(2) << setfill('0') << (unsigned int)controlcode[0]<< endl;
			cout << hex << setw(2) << setfill('0') << (unsigned int)controlcode[1]<< dec << endl;
			*/
			switch(controlcode[0]){
			case 0x01: //Send Time Code
				internal_timecode=controlcode[1];
				throw SSDTPException(SSDTPException::TimecodeReceived);
				break;
			case 0x02: //Got Time Code
				internal_timecode=controlcode[1];
				gotTimeCode(internal_timecode);
				break;
			}
		}else{
			//data
			/*
			cerr << "data" << endl;cout.flush();
			cout << "rheader[0] : " << (unsigned int)rheader[0] << endl;
			*/
			//cout << "hsize : " << hsize << endl;
			hsize=0;
			while(hsize!=10){
				int result=datasocket->receive(rheader+2+hsize,10-hsize);
				hsize+=result;
			}
			//cout << "size part : ";
			/*for(int i=0;i<10;i++){
				cout << hex << setw(2) << setfill('0') << (unsigned int)rheader[i+2] << " ";
			}*/
			//cout << dec << endl;
			//cout << "hsize : " << hsize << endl;
			for(unsigned int i=2;i<12;i++){
				flagment_size=flagment_size*0x100+rheader[i];
				//cout << "rheader " << i << " " << setw(2) << setfill('0') << hex << (unsigned int)rheader[i] << dec << endl;
			}
			//cout << "flagment_size : " << dec << flagment_size << endl;
			unsigned char* data_pointer=&(data->at(0));
			while(received_size!=flagment_size){
				int result=datasocket->receive(data_pointer+size+received_size,flagment_size-received_size);
				received_size+=result;
			}
			//cout << received_size << "bytes received" << endl;
			size+=received_size;
		}
	}
	data->resize(size);
#endif

#ifdef ORIGINAL_SSDTP1
	int index=0;
	int sizesize;

	data->resize(SSDTPModule::BufferSize);
	receivebuffer=(unsigned char*)&data->at(0);

	//data or control
	int receivedsize=0;
	try{
		receivedsize=datasocket->receive(receivebuffer,1);
	}catch(...){
		throw SSDTPException(SSDTPException::SequenceError);
	}
	if(receivedsize==0){
		throw SSDTPException(SSDTPException::SequenceError);
	}

	receivedsize=0;
	if(receivebuffer[0]==SSDTPModule::DataFlag){
		//determine packet size
		receivedsize=0;
		while(receivedsize<SSDTPModule::LengthOfSizePart){
			try{
				receivedsize+=datasocket->receive(receivebuffer+receivedsize,SSDTPModule::LengthOfSizePart-receivedsize);
			}catch(...){
				throw SSDTPException(SSDTPException::SequenceError);
			}
		}
		receivebuffer[SSDTPModule::LengthOfSizePart]=0x00;
		int size=atoi((const char*)receivebuffer);

		//receive data
		receivedsize=0;
		while(receivedsize<size){
			try{
				receivedsize+=datasocket->receive(receivebuffer+receivedsize,size-receivedsize);
			}catch(...){
				throw SSDTPException(SSDTPException::SequenceError);
			}
		}
		data->resize(size);
	}else{
		receivedsize=datasocket->receive(receivebuffer,1);
		if(receivebuffer[0]==SSDTPModule::EEP){
			throw SSDTPException(SSDTPException::EEP);
		}else if(receivebuffer[0]==SSDTPModule::Timecode){
			try{
				receivedsize=datasocket->receive(receivebuffer,1);
			}catch(...){
				throw SSDTPException(SSDTPException::SequenceError);
			}
			internal_timecode=receivebuffer[0];
			throw SSDTPException(SSDTPException::TimecodeReceived);
		}
	}
#endif

#ifdef SSDTP1

	int sizesize;
	int r_state=0;
	int r_mode_selector=SSDTPModule::Null;
	int r_sizepart_index=0;
	int r_size;

	int r_data_index;
	int r_timecode_footer;

	//data or control
	receive_loop:
	if(receivedsize==rbuf_index){
		try{
			receivedsize=datasocket->receive(receivebuffer,SSDTPModule::BufferSize);
			rbuf_index=0;
		}catch(IPSocketException e){
			if(e.getStatus()=="IPSocket::receive() TimeOut"){
				goto receive_loop;
			}
			cout << "IPSocket disconnected" << endl;
#ifdef SYSTEM_TYPE_POSIX
			exit(-1);
#endif
#ifdef SYSTEM_TYPE_T_KERNEL
			throw SSDTPException(SSDTPException::SequenceError);
#endif
		}catch(...){
			throw SSDTPException(SSDTPException::SequenceError);
		}
		if(receivedsize<=0){
			throw SSDTPException(SSDTPException::SequenceError);
		}
	}

	while(rbuf_index!=receivedsize){
		if(r_mode_selector==SSDTPModule::Null){
			if(receivebuffer[rbuf_index]==SSDTPModule::DataFlag){
				r_mode_selector=SSDTPModule::DataFlag;
				r_sizepart_index=0;
			}else{
				r_mode_selector=SSDTPModule::ControlFlag;
			}
			r_state=0;
			rbuf_index++;
		}
		if(r_mode_selector==SSDTPModule::DataFlag){
			switch(r_state){
			case 0: //size part
				//determine packet size
				while(r_sizepart_index<SSDTPModule::LengthOfSizePart){
					if(rbuf_index!=receivedsize){
						r_tmp[r_sizepart_index]=receivebuffer[rbuf_index];
						rbuf_index++;
						r_sizepart_index++;
					}else{
						goto receive_loop;
					}
				}
				r_tmp[SSDTPModule::LengthOfSizePart]=0x00;
				r_size=atoi((const char*)r_tmp);
				//debug
				if(r_size==0){
					cout << "r_size=0:" << (const char*)r_tmp << endl;
				}
				data->resize(r_size);
				r_data_index=0;
				r_state=1;
			case 1: //data part
				if(r_size<=receivedsize-rbuf_index){
					memcpy((void*)&(data->at(r_data_index)),(void*)(receivebuffer+rbuf_index),r_size);
					rbuf_index+=r_size;
					return data->size();
				}else{
					int available_size=receivedsize-rbuf_index;
					if(available_size!=0){
						memcpy((void*)&(data->at(r_data_index)),(void*)(receivebuffer+rbuf_index),receivedsize-rbuf_index);
						rbuf_index+=receivedsize-rbuf_index;
						r_data_index+=receivedsize-rbuf_index;
					}
					goto receive_loop;
				}
				break;
			}
		}else{
			control_code_fsm:
			switch(r_state){
			case 0://control code
				if(rbuf_index!=receivedsize){
					if(receivebuffer[rbuf_index]==SSDTPModule::Timecode){
						rbuf_index++;
						r_state=1;
					}else if(receivebuffer[rbuf_index]==SSDTPModule::EEP){
						rbuf_index++;
						throw SSDTPException(SSDTPException::EEP);
					}
				}else{
					goto receive_loop;
				}
			case 1://TimeCode
				if(rbuf_index!=receivedsize){
					internal_timecode=receivebuffer[rbuf_index];
					rbuf_index++;
					r_timecode_footer=0;
					r_state=2;
				}else{
					goto receive_loop;
				}
			case 2://TimeCode (remaining 15bytes)
				if(r_timecode_footer!=TIMECODE_FOOTER_SIZE){
					if(rbuf_index!=receivedsize){
						if(TIMECODE_FOOTER_SIZE-r_timecode_footer<=receivedsize-rbuf_index){
							rbuf_index+=TIMECODE_FOOTER_SIZE-r_timecode_footer;
							r_timecode_footer=TIMECODE_FOOTER_SIZE;
							return -10;
						}else{
							rbuf_index=receivedsize;
							r_timecode_footer+=receivedsize-rbuf_index;
							goto receive_loop;
						}
					}else{
						goto receive_loop;
					}
				}

			}
		}
	}
	goto receive_loop;
#endif

	return 0;	// not reached
}

void SSDTPModule::sendEEP() throw(SSDTPException) {
	throw SSDTPException(SSDTPException::NotImplemented);
}

void SSDTPModule::sendTimeCode(unsigned char timecode) throw(SSDTPException) {
#ifdef SSDTP2
	sendmutex.lock();
	sendbuffer[0]=SSDTPModule::ControlFlag_SendTimeCode;
	sendbuffer[1]=0x00; //Reserved
	for(unsigned int i=0;i<LengthOfSizePart-1;i++){
		sendbuffer[i+2]=0x00;
	}
	sendbuffer[11]=0x02; //2bytes = 1byte timecode + 1byte reserved
	sendbuffer[12]=timecode;
	sendbuffer[13]=0;
	datasocket->send(sendbuffer,14);
	sendmutex.unlock();
#endif
#ifdef SSDTP2_OLD
	sendmutex.lock();
	int index=0;
	sendbuffer[index]=SSDTPModule::ControlFlag;
	index++;
	sendbuffer[index]=0x00; //port number (not implemented)
	index++;
	sendbuffer[index]=Control_SendTimeCode;
	index++;
	sendbuffer[index]=timecode;
	index++;
	datasocket->send(sendbuffer,index);
	sendmutex.unlock();
#endif
#ifdef SSDTP1
	sendmutex.lock();
	int index=0;
	sendbuffer[index]=SSDTPModule::ControlFlag;
	index++;

	sendbuffer[index]=SSDTPModule::Timecode;
	index++;
	sendbuffer[index]=timecode;
	index++;
	for(int i=0;i<TIMECODE_FOOTER_SIZE;i++){
		index++;
		sendbuffer[index]=0xab;
	}
	datasocket->send(sendbuffer,index);
	sendmutex.unlock();
#endif
}

unsigned char SSDTPModule::getTimeCode() throw(SSDTPException){
	return internal_timecode;
}

void SSDTPModule::setTimeCodeAction(SpaceWireTimeCodeAction* action){
	timecodeaction=action;
}

void SSDTPModule::setTxFrequency(double frequencyInMHz){

}

void SSDTPModule::gotTimeCode(unsigned char timecode){
	if(timecodeaction!=NULL){
		timecodeaction->doAction(timecode);
	}else{
		cout << "SSDTPModule::gotTimeCode(): Got TimeCode : " << hex << setw(2) << setfill('0') << (unsigned int)timecode << dec << endl;
	}
}

void SSDTPModule::registerRead(unsigned int address){
	//send command

	//wait reply
}

void SSDTPModule::registerWrite(unsigned int address,vector<unsigned char> data){
#ifdef SSDTP2
	//send command
	sendmutex.lock();
	sendbuffer[0]=ControlFlag_RegisterAccess_WriteCommand;

	sendmutex.unlock();
#endif
}

void SSDTPModule::setTxDivCount(unsigned char txdivcount){
#ifdef SSDTP2
	sendmutex.lock();
	sendbuffer[0]=SSDTPModule::ControlFlag_ChangeTxSpeed;
	sendbuffer[1]=0x00; //Reserved
	for(unsigned int i=0;i<LengthOfSizePart-1;i++){
		sendbuffer[i+2]=0x00;
	}
	sendbuffer[11]=0x02; //2bytes = 1byte txdivcount + 1byte reserved
	sendbuffer[12]=txdivcount;
	sendbuffer[13]=0;
	datasocket->send(sendbuffer,14);
	sendmutex.unlock();
#endif
#ifdef SSDTP1
	throw SSDTPException(SSDTPException::NotImplemented)
#endif
}


/** History
 * 2008-06-xx file created (Takayuki Yuasa)
 * 2008-12-17 TimeCode implemented (Takayuki Yuasa)
 * 2010-05-xx protocol updated (SSDTP2) for hardware implementation (Takayuki Yuasa)
 * 2010-06-03 multiple port support (Takayuki Yuasa)
 * 2010-07-01 TimeCode and RegisterAccess for SpaceWire-to-GigabitEther (Takayuki Yuasa)
 */
