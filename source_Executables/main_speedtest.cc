#include "SpaceWire.hh"
#include "RMAP.hh"

#include "Condition.hh"
#include "Mutex.hh"
#include "Thread.hh"

#include <exception>
#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
#include <string>

#ifdef SYSTEM_TYPE_POSIX
#include <sys/time.h>
#include <time.h>
#endif
#ifdef SYSTEM_TYPE_T_KERNEL
#include "btron/tkcall.h"
#include <util/tmonitor.h>
#endif

#include <stdlib.h>
using namespace std;

class Main {
private:
	static SpaceWireIF* spacewireif;
	static RMAPEngine* rmapengine;
	static RMAPSocket* rmapsocket;
	static RMAPDestination* rmapdestination;
	SpaceWireCLI cli;
	int menu; //selected command number

	static unsigned int address;
	static unsigned int length; //in a unit of byte
	static unsigned int ramsize;
	static unsigned int iteration;
	static unsigned int displayfrequency;

#ifdef SYSTEM_TYPE_POSIX
	struct timeval starttime,endtime,duration;
#endif
public:
	Main() {
		spacewireif = SpaceWireIFImplementations::selectInstanceFromCommandLineMenu();
		spacewireif->initialize();
		rmapdestination = new RMAPDestination();

		bool result = spacewireif->open();
		if (result != true) {
			cout << "SpaceWireIF open error" << endl;
			exit(-1);
		}
	}
	~Main() {
		spacewireif->close();
		delete spacewireif;
	}
	void run() {
		showTitle();

		loop:
		//show menu
		showMenu();
		//selection
		cli.ask2_int(cin, menu, cout, "select > ");
		//process selected menu
		processCommand(menu);
		if (menu != 9) {
			goto loop;
		}
	}
private:
	void showTitle() {
		cout << "############################################" << endl;
		cout << "###         SpaceWire speed test         ###" << endl;
		cout << "############################################" << endl << endl;
	}

	void showMenu() {
		cout << "Menu : " << endl;
		cout << "  SDRAM access (via SpaceWireIF)  [1]" << endl;
		cout << "  SDRAM access (via RMAPSocket)   [2]" << endl;
		cout << "  Change RMAP Destination Info    [8]" << endl;
		cout << "  Toggle Debug Mode (now " << (spacewireif->isDebugMode() ? "On) " : "Off)") << "     [0]" << endl;
		cout << "  Quit                            [9]" << endl << endl;
	}

	void processCommand(int menu) {
		switch (menu) {
		case 1: //via SpaceWireIF
			viaSpaceWireIF();
			break;
		case 2: //via RMAPEngine/RMAPSocket
			viaRMAPSocket();
			break;
		case 8: //change RMAPDestination
			setDestination();
			break;
		case 0:
			spacewireif->toggleDebugMode();
			break;
		case 100:
			measureMutex();
			break;
		case 9: //Quit
			break;
		}
	}
private:
	void measureMutex() {
#ifdef SYSTEM_TYPE_POSIX
		gettimeofday(&starttime,NULL);
#endif
#ifdef SYSTEM_TYPE_T_KERNEL
		SYSTIM pk_tim_start;
		SYSTIM pk_tim_end;
		ER ercd;
		ercd=tk_get_tim(&pk_tim_start);
#endif	
		Mutex mutex;
		for (int i = 0; i < 100000; i++) {
			mutex.lock();
			mutex.unlock();
		}
		double millis = 0;

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

		cout << "Mutex lock-unlock 10^5times in" << dec << millis / 1000 << "sec" << endl;

	}
	void viaSpaceWireIF() {
		setParameters();

		RMAPPacket commandpacket;
		commandpacket.setRMAPDestination(*rmapdestination);
		commandpacket.setCommandPacket();
		commandpacket.setReadMode();
		commandpacket.setAddress(address);
		commandpacket.setLength(length);

#ifdef SYSTEM_TYPE_POSIX
		gettimeofday(&starttime,NULL);
#endif
#ifdef SYSTEM_TYPE_T_KERNEL
		SYSTIM pk_tim_start;
		SYSTIM pk_tim_end;
		ER ercd;
		ercd=tk_get_tim(&pk_tim_start);
#endif		
		cout << endl;
		cout << "Start Read Access" << endl;
		unsigned int currentaddress = address;
		RMAPPacket replypacket;
		replypacket.disableCRCCheck();
		vector<unsigned char> receivedpacketvector;
		vector<unsigned char>* data;
		for (unsigned int i = 0; i < iteration; i++) {
			commandpacket.setAddress(currentaddress);
			commandpacket.createPacket();
			try {
				/*
				 data=new vector<unsigned char>;
				 spacewireif->send(commandpacket.getPacket());
				 spacewireif->receive(data);
				 replypacket.setPacketBuffer(data);*/
				spacewireif->send(commandpacket.getPacket());
				spacewireif->receive(replypacket.getPacketBuffer());
				replypacket.interpretPacket();
			} catch (SpaceWireException e) {
				cout << "Exception in SpaceWire access (" << dec << i + 1 << ")" << endl;
				e.dump();
				exit(-1);
			} catch (RMAPException e) {
				cout << "Exception in interpretation of replied packet (" << dec << i + 1 << ")" << endl;
				SpaceWireUtilities::dumpPacket(data);
				e.dump();
				exit(-1);
			}
			if (i % displayfrequency == 0) {
				cout << "Done (" << dec << i + 1 << ")" << endl;
			}
			if (currentaddress > address + ramsize - length) {
				cout << "Turnup" << endl;
				currentaddress = address;
			} else {
				currentaddress = currentaddress + length;
			}
		}

		//show elapsed time and transfer speed
		double millis = 0;

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

		cout << "Transferred " << (double) (length * iteration) / 1000. << "kBytes in " << dec << millis / 1000
				<< "sec" << endl;
		cout << "Bit-rate    " << (double) (length * iteration) / 1000. / (millis / 1000) * 8 << "kbit/s" << endl;

		cout << "Completed" << endl << endl;
	}
	void viaRMAPSocket() {
		setParameters();
		rmapengine = new RMAPEngine(spacewireif);
		rmapengine->start();
		cout << endl;
		cout << "Start Read Access" << endl;

		class RMAPReadViaRMAPSocketThread: public Thread {
		private:
			int threadid;
			RMAPSocket* rmapsocket;
			unsigned int currentaddress;
			bool done;
			double millis;
#ifdef SYSTEM_TYPE_POSIX
			struct timeval starttime,endtime,duration;
#endif
#ifdef SYSTEM_TYPE_T_KERNEL
			SYSTIM pk_tim_start;
			SYSTIM pk_tim_end;
#endif
		public:
			RMAPReadViaRMAPSocketThread(int threadid) {
				this->threadid=threadid;
				rmapsocket = rmapengine->openRMAPSocketTo(*rmapdestination);
				rmapsocket->getRMAPDestination().dump();
				cout << "RMAPSocket " << rmapsocket->getSocketID() << endl;
				millis = 0;
				done = false;
			}
			~RMAPReadViaRMAPSocketThread() {
				rmapengine->closeRMAPSocket(rmapsocket);
			}
			bool isDone() {
				return done;
			}
			double getDuration() {
				return millis;
			}
			void run() {
				vector<unsigned char>* readdata;
				currentaddress = address;
#ifdef SYSTEM_TYPE_POSIX
				gettimeofday(&starttime,NULL);
#endif
#ifdef SYSTEM_TYPE_T_KERNEL
				ER ercd;
				ercd=tk_get_tim(&pk_tim_start);
#endif
				rmapsocket->enableTimeout(1000);
				for (unsigned int i = 0; i < iteration; i++) {
					try {
						try {
							readdata = rmapsocket->read(currentaddress, length);
						} catch (RMAPException e) {
							if(e.getStatus()==RMAPException::Timedout){
								cout << "timeout in thread " << threadid << endl;
							}else{
								cout << "Exception in RMAPSocket access (" << dec << i + 1 << ")" << endl;
								e.dump();
								::exit(-1);
							}
						} catch (SpaceWireException e) {
							cout << "Exception in SpaceWire access (" << dec << i + 1 << ")" << endl;
							e.dump();
							::exit(-1);
						}
						if (readdata->size() != length) {
							cout << "received packet length error (" << dec << i + 1 << ")" << endl;
							::exit(-1);
						}
						if (i % displayfrequency == 0) {
							cout << "Done " << "Thread ID=" << threadid << "(" << dec << i + 1 << ")" << endl;
						}
						if (currentaddress > address + ramsize - length) {
							cout << "Turnup" << endl;
							currentaddress = address;
						} else {
							currentaddress = currentaddress + length;
						}
					} catch (exception e) {
						cout << e.what() << endl;
					}
				}
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
				cout << "thread " << threadid << " done." << endl;
				done=true;
			}
		};

		//ask thread number
		cout << "Input thread number > ";
		int numthreads;
		cin >> numthreads;
		vector<Thread*> threads;
		for (int i = 0; i < numthreads; i++) {
			RMAPReadViaRMAPSocketThread* thread = new RMAPReadViaRMAPSocketThread(i);
			threads.push_back((Thread*) thread);
		}
		for (int i = 0; i < numthreads; i++) {
			cout << "thread " << i << " started..." << endl;
			threads.at(i)->start();
		}

		//check if all threads done
		bool alldone = false;
		Condition c;
		while (!alldone) {
			c.wait(1000);
			cout << "waiting..." << endl;
			alldone = true;
			for (unsigned int i = 0; i < threads.size(); i++) {
				if (((RMAPReadViaRMAPSocketThread*) threads.at(i))->isDone() == false) {
					alldone = false;
				}
			}
		}

		//show elapsed time and transfer speed
		double millis = 0;
		for (unsigned int i = 0; i < threads.size(); i++) {
			millis += ((RMAPReadViaRMAPSocketThread*) threads.at(i))->getDuration();
		}
		millis=millis/threads.size();
		cout << "Using " << numthreads << " thread..." << endl;
		cout << "Transferred " << (double) (length * iteration * numthreads) / 1000. << "kBytes in " << dec << millis
				/ 1000 << "sec" << endl;
		cout << "Bit-rate    " << (double) (length * iteration * numthreads) / 1000. / (millis / 1000) * 8 << "kbit/s"
				<< endl;

		cout << "Completed" << endl << endl;

		for (int i = 0; i < numthreads; i++) {
			delete threads.at(i);
		}

		rmapengine->stop();
		delete rmapengine;
	}
private:
	void setParameters() {
		cout << "initial address of read access" << endl;
		address = askAddress();
		cout << "ram size (where turnup occurs)" << endl;
		ramsize = askRamSize();
		cout << "length of each access" << endl;
		length = askLength();
		cout << "number of iteration" << endl;
		iteration = askIteration();
		cout << "display frequency" << endl;
		displayfrequency = askDisplayFrequency();
	}
	unsigned int askAddress() {
		unsigned int value;
		cli.ask2_hex(cin, value, cout, "address in hex > ");
		return value;
	}
	unsigned int askRamSize() {
		int value;
		askramsizeloop: cli.ask2_int(cin, value, cout, "in decimal kB > ");
		if (value < 0) {
			goto askramsizeloop;
		} else {
			return value * 1000;
		}
	}
	unsigned int askLength() {
		int value;
		asklengthloop: cli.ask2_int(cin, value, cout, "length in decimal > ");
		if (value < 0) {
			goto asklengthloop;
		} else {
			return value;
		}
	}
	unsigned int askIteration() {
		int value;
		askiterationloop: cli.ask2_int(cin, value, cout, "iteration number in decimal > ");
		if (value < 0) {
			goto askiterationloop;
		} else {
			return value;
		}
	}
	unsigned int askDisplayFrequency() {
		int value;
		askfrequencyloop: cli.ask2_int(cin, value, cout, "show degree of process per N accesses : N > ");
		if (value < 0) {
			goto askfrequencyloop;
		} else {
			return value;
		}
	}
private:
	void setDestination() {
		delete rmapdestination;
		rmapdestination = RMAPDestinationKnownDestinations::selectInstanceFromCommandLineMenu();
	}
};
unsigned int Main::address = 0;
unsigned int Main::length = 4000; //in a unit of byte
unsigned int Main::ramsize = 16000000;
unsigned int Main::iteration = 1000;
unsigned int Main::displayfrequency = 100;
SpaceWireIF* Main::spacewireif=0;
RMAPEngine* Main::rmapengine=0;
RMAPSocket* Main::rmapsocket=0;
RMAPDestination* Main::rmapdestination=0;


int main(int argc, char* argv) {
	try {
		Main main;
		main.run();
	} catch (...) {
		cout << "Exception in main" << endl;
	}
}

/** History
 * 2008-05-xx file created (Takayuki Yuasa)
 * 2008-09-02 modified to SpaceWireIFImplementations
 *            destination change implemented (Takayuki Yuasa)
 * 2008-09-04 use SpaceWireIFImplementations::selectInstanceFromCommandLineMenu() (Takayuki Yuasa)
 * 2008-10-07 setDestination() modified to use RMAPDestinationKnownDestinations class (Takayuki Yuasa)
 */
