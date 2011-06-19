#include "SpaceWireIFOverIPServer.hh"
#include "Condition.hh"
using namespace std;

//#define DEBUG

SpaceWireIFOverIPServer::SpaceWireIFOverIPServer(unsigned int newipportnumber) :
	SpaceWireIF(), ipportnumber(newipportnumber) {
	cout << "SpaceWireIFOverIPServer::SpaceWireIFOverIPServer()" << endl;
}

SpaceWireIFOverIPServer::~SpaceWireIFOverIPServer() {
	listeningsocket->close();
}

bool SpaceWireIFOverIPServer::initialize() throw (SpaceWireException) {
	initializationmutex.lock();
	initialized = true;
	listeningsocket = new IPServerSocket(ipportnumber);
	listeningsocket->open();
	initializationmutex.unlock();
	return true;
}

bool SpaceWireIFOverIPServer::open() throw (SpaceWireException) {
	openclosemutex.lock();
	try {
		acceptedsocket = listeningsocket->accept(); //copy byte-to-byte
	} catch (IPSocketException e) {
		cout << "could not accept a client" << endl;
	}
	cout << "Connected" << endl;

	ssdtp = new SSDTPModule(acceptedsocket);

	return true;
}

bool SpaceWireIFOverIPServer::open(int portnumber) throw (SpaceWireException) {
	throw SpaceWireException(SpaceWireException::NotImplemented);
}

void SpaceWireIFOverIPServer::close() {
	acceptedsocket->close();
	listeningsocket->close();
	delete acceptedsocket;
	delete listeningsocket;
	delete ssdtp;
	openclosemutex.unlock();
}

void SpaceWireIFOverIPServer::startLinkInitializationSequenceAsAMasterNode() throw (SpaceWireException) {
}
void SpaceWireIFOverIPServer::startLinkInitializationSequenceAsASlaveNode() throw (SpaceWireException) {
}
void SpaceWireIFOverIPServer::stopLink() throw (SpaceWireException) {
}

void SpaceWireIFOverIPServer::resetStateMachine() throw (SpaceWireException) {
}
void SpaceWireIFOverIPServer::clearSendFIFO() throw (SpaceWireException) {
}
void SpaceWireIFOverIPServer::clearReceiveFIFO() throw (SpaceWireException) {
}

bool SpaceWireIFOverIPServer::reset() throw (SpaceWireException) {
	close();
	open();
	return true;
}

void SpaceWireIFOverIPServer::finalize() throw (SpaceWireException) {
	SpaceWireIF::finalize();
}

void SpaceWireIFOverIPServer::send(vector<unsigned char>* packet) throw (SpaceWireException) {
#ifdef DEBUG
	cout << "SpaceWireIFOverIPServer::send() invoked" << endl;
#endif
	try {
		ssdtp->send(packet);
	} catch (SSDTPException e) {
		if (e.getStatus() == SSDTPException::Timeout) {
			throw SpaceWireException(SpaceWireException::Timeout);
		} else {
			throw SpaceWireException(SpaceWireException::Disconnected);
		}
	}

#ifdef DEBUG
	cout << "SpaceWireIFOverIPServer::send() completed" << endl;
#endif
}

vector<unsigned char> SpaceWireIFOverIPServer::receive() throw (SpaceWireException) {
#ifdef DEBUG
	cout << "SpaceWireIFOverIPServer::receive() invoked" << endl;
#endif
	try {
		return ssdtp->receive();
	} catch (SSDTPException e) {
		if (e.getStatus() == SSDTPException::Timeout) {
			throw SpaceWireException(SpaceWireException::Timeout);
		}
		throw SpaceWireException(SpaceWireException::Disconnected);
	} catch (IPSocketException e) {
		if (e.getStatus() == IPSocketException::Timeout) {
			throw SpaceWireException(SpaceWireException::Timeout);
		}
		throw SpaceWireException(SpaceWireException::Disconnected);
	}
#ifdef DEBUG
	cout << "SpaceWireIFOverIPServer::receive() waiting" << endl;
#endif
}

void SpaceWireIFOverIPServer::receive(vector<unsigned char>* packet) throw (SpaceWireException) {
	*packet = receive();
}

void SpaceWireIFOverIPServer::abort() throw (SpaceWireException) {

}

SpaceWireAsynchronousStatus SpaceWireIFOverIPServer::asynchronousReceive() throw (SpaceWireException) {
	return *(new SpaceWireAsynchronousStatus);
}
SpaceWireAsynchronousStatus SpaceWireIFOverIPServer::asynchronousSend(vector<unsigned char>* packet)
		throw (SpaceWireException) {
	return *(new SpaceWireAsynchronousStatus);
}
void SpaceWireIFOverIPServer::waitCompletionOfAsynchronousOperation(SpaceWireAsynchronousStatus status)
		throw (SpaceWireException) {
}

void SpaceWireIFOverIPServer::sendTimeCode(unsigned char flag_and_timecode) throw (SpaceWireException) {
	ssdtp->sendTimeCode(flag_and_timecode);
}

unsigned char SpaceWireIFOverIPServer::getTimeCode() throw (SpaceWireException) {
	return ssdtp->getTimeCode();
}

/** History
 * 2008-08-26 file created (Takayuki Yuasa)
 * 2008-12-17 added TimeCode related methods (Takayuki Yuasa)
 */
