/*
 * RMAPSocketConnectionTester.hh
 *
 *  Created on: 2010/06/08
 *      Author: yuasa
 */

#ifndef RMAPSOCKETCONNECTIONTESTER_HH_
#define RMAPSOCKETCONNECTIONTESTER_HH_

#include "RMAPSocket.hh"
#include "Thread.hh"
using namespace std;

/** This class tests if an newly opened RMAPSocket successfully works,
 * or in other words, if user can RMAP-read/RMAP-write to memory via
 * the RMAPSocket. This might be useful in initialization part of
 * user program.
 */
class RMAPSocketConnectionTester : public Thread {
private:
	RMAPSocket* rmapsocket;
	unsigned int duration;
	unsigned int address;
	unsigned int length;
	bool succeeded;
	bool started;
	Mutex m;
public:
	/** Constructs an instance.
	 * @param rmapsocket an instance of RMAPSocket to be tested
	 * @param address memory address of the RMAP node to be accessed for test
	 * @param length access length in bytes
	 */
	RMAPSocketConnectionTester(RMAPSocket* rmapsocket,unsigned int address,unsigned int length);

	~RMAPSocketConnectionTester();

	/** Sets time out duration to be used in this instance.
	 * @param duration duration in milli second
	 */
	void setTimeoutDuration(unsigned int duration);

	/** Starts to access the address using the provided RMAPSocket.
	 */
	void run();

	/** Stops trying to access.
	 */
	void stop();

	/** Checks the access status.
	 * @return true if test access has been successfully done
	 */
	bool isSucceeded();
};

#endif /* RMAPSOCKETCONNECTIONTESTER_HH_ */
