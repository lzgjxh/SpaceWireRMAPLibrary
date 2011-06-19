#ifndef SPACEWIREEXCEPTION_HH_
#define SPACEWIREEXCEPTION_HH_

#include <iostream>
using namespace std;

/** An exception status class passed when
 * an exception happens in SpaceWire processes.
 */
class SpaceWireException {
public:
	static enum {
		OpenFailed,
		NotOpened,
		NoException,
		NotImplemented,
		TransferNotStarted,
		TransferStarted,
		TransferSuccess,
		TransferErrorDisconnect,
		TransferErrorUnkown,
		TransferErrorTimeout,
		TransferErrorMemoryOverflow,
		SendFailed,
		ReceiveFailed,
		SizeIncorrect,
		Disconnected,
		DeviceDriverFailed,
		NoDevice,
		SendTimeCodeError,
		GetTimeCodeError,
		Timeout
	} SpaceWireExceptionStates;
private:
	int status;
public:
	/** Constructor.
	 */
	SpaceWireException();

	/** Constructor with status initialization.
	 * @param newstatus exception status
	 */
	SpaceWireException(int newstatus);

	/** Setter for status of this class.
	 * @param newstatus exception status
	 */
	void setStatus(int newstatus);

	/** Getter for status of this class.
	 * @return exception status
	 */
	int getStatus();

	/** Dump this class.
	 */
	void dump();
};
#endif /*SPACEWIREEXCEPTION_HH_*/

/** History
 * 2008-05-xx file created (Takayuki Yuasa)
 * 2008-10-08 added dump() implementation (Takayuki Yuasa)
 * 2008-12-17 added SendTimeCodeError and GetTimeCodeError (Takayuki Yuasa)
 */
