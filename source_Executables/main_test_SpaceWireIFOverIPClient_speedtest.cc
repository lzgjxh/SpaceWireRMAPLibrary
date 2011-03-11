#include "SpaceWireIFOverIPClient.hh"

#include <iostream>

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
	string hostname;
	unsigned int portnumber;
	unsigned int packetsize;
	unsigned int iterationnumber;
public:
	Main(string newhostname, unsigned int newportnumber, unsigned int packetsize, unsigned int iterationnumber) :
		hostname(newhostname), portnumber(newportnumber), packetsize(packetsize), iterationnumber(iterationnumber) {
		//Initialization
	}
public:
	void run() {
		//initialize IF
		SpaceWireIF* spacewireif = new SpaceWireIFOverIPClient(hostname, portnumber);
		spacewireif->initialize();

		//connect to server IF
		spacewireif->open();

		vector<unsigned char> data;
		unsigned int tmp;

		//packet size
		tmp = packetsize;
		while (tmp != 0) {
			data.push_back(tmp % 0x100);
			tmp = tmp / 0x100;
		}
		cout << "sending packet size(" << packetsize << ")..." << endl;
		spacewireif->send(&data);

		//iteration number
		data.clear();
		tmp = iterationnumber;
		while (tmp != 0) {
			data.push_back(tmp % 0x100);
			tmp = tmp / 0x100;
		}
		cout << "sending iteration number(" << iterationnumber << ")..." << endl;
		spacewireif->send(&data);

#ifdef SYSTEM_TYPE_POSIX
		struct timeval starttime,endtime,duration;
		gettimeofday(&starttime,NULL);
#endif
#ifdef SYSTEM_TYPE_T_KERNEL
		SYSTIM pk_tim_start;
		SYSTIM pk_tim_end;
		ER ercd;
		ercd=tk_get_tim(&pk_tim_start);
#endif

		//receive
		cout << "receiving...";
		for (unsigned int i = 0; i < iterationnumber; i++) {
			spacewireif->receive(&data);
			//SpaceWireUtilities::dumpPacket(&data);
		}
		cout << "done" << endl;

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

		cout << "Transferred " << (double) (packetsize * iterationnumber) / 1000. << "kBytes in " << dec << millis
				/ 1000 << "sec" << endl;
		cout << "Bit-rate    " << (double) (packetsize * iterationnumber) / 1000. / 1000. / (millis / 1000) * 8 << "Mbit/s"
				<< endl;

		cout << "Completed" << endl << endl;

		//finalization
		//spacewireif->close();
	}
};

int main(int argc, char* argv[]) {
	if (argc == 5) {
		string hostname = argv[1];
		unsigned int portnumber = atoi(argv[2]);
		unsigned int packetsize = atoi(argv[3]);
		unsigned int iterationnumber = atoi(argv[4]);
		Main main(hostname, portnumber, packetsize, iterationnumber);
		main.run();
	} else {
		cout << "give hostname and portnumber" << endl;
	}
	return 0;
}

/** History
 * 2008-08-26 file created (Takayuki Yuasa)
 */
