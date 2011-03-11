#include "Message.hh"

using namespace std;

Message::Message(){
	
}

Message::~Message(){
}

void Message::send(string message) throw (SynchronousException){
	if(message.size()>Message::BufferSize){
		throw SynchronousException("Message::send() : too large message is given");
	}
	string* newmessage=new string();
	*newmessage=message;
	messagequeuemutex.lock();
	messagequeue.push(newmessage);
	messagequeuemutex.unlock();
	condition.signal();
}

string Message::receive() throw (SynchronousException){
	return this->receive(0);
}

string Message::receive(int timeoutInMillisecond) throw (SynchronousException){
	int ellapsedtime=0;
	loop:
	messagequeuemutex.lock();
	if(messagequeue.size()!=0){
		string* receivedmessage=messagequeue.front();
		messagequeue.pop();
		messagequeuemutex.unlock();
		return *receivedmessage;
	}
	messagequeuemutex.unlock();
	condition.wait(Message::WaitInterval);
	if(timeoutInMillisecond!=0){
		ellapsedtime+=Message::WaitInterval;
		if(ellapsedtime>timeoutInMillisecond){
			throw SynchronousException("Message::receive() Timeout");
		}
	}
	goto loop;
}

/** History
 * 2008-09-xx file created (Toshinari Hagihara)
 * 2008-10-03 restructuring (Takayuki Yuasa)
 */
