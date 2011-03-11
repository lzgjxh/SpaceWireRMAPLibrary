#include "Message.hh"

using namespace std;

Message::Message(){
	sendmsgbuf = new char[Message::BufferSize];
	recvmsgbuf = new char[Message::BufferSize];

	cmbf.exinf = NULL;
	cmbf.mbfatr = TA_TFIFO; //or TA_TPRI
	cmbf.bufsz = Message::DepthOfMessageQueue*Message::BufferSize;
	cmbf.maxmsz = Message::BufferSize;
	msqid = tk_cre_mbf(&cmbf);
	if(msqid<E_OK){
		throw SynchronousException("Message::Message() msgbuf creation error");
	}
}

Message::~Message(){
	delete sendmsgbuf;
	delete recvmsgbuf;
	tk_del_mbf(msqid);
}

void Message::send(string message) throw (SynchronousException){
	if(message.size()>Message::BufferSize){
		throw SynchronousException("Message::send() : too large message is given");
	}
	
	sendmutex.lock();
	strcpy(sendmsgbuf,message.c_str());
	status=tk_snd_mbf(msqid, (unsigned char*)sendmsgbuf, Message::BufferSize, 100);

	if(status < 0){
		cout << "Message::send() sending error :" ;
		cout << "errno=" << status << endl;
	}
	sendmutex.unlock();
}

string Message::receive() throw (SynchronousException){
	return this->receive(TMO_FEVR);
}

string Message::receive(int timeoutInMillisecond) throw (SynchronousException){
	string message;
	recvmutex.lock();
	status=tk_rcv_mbf(msqid, (unsigned char*)recvmsgbuf, timeoutInMillisecond);
	if(status < 0){
		throw SynchronousException("Message::receive() receiving error");
	}
	message=(char*)recvmsgbuf;
	recvmutex.unlock();

	return message;
}

/** History
 * 2008-09-xx file created (Toshinari Hagihara)
 * 2008-10-03 restructuring (Takayuki Yuasa)
 */