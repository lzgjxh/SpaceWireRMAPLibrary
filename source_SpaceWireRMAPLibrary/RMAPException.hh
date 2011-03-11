#ifndef RMAPEXCEPTION_HH_
#define RMAPEXCEPTION_HH_

#include <iostream>
#include <string>
using namespace std;

/** An exception status class passed when
 * an exception happens in RMAP process.
 */ 
class RMAPException {
private:
	int status;
public:
	static enum {
		NotAnRMAPPacket,
		InvalidHeaderCRC,
		InvalidDataCRC,
		InvalidDestination,
		TooManySockets,
		SocketIDOutOfRange,
		TooManyRequests,
		TIDOutOfRange,
		RMAPUnsuccessfull,
		RMAPEngine_MultipleTransactionScheme_Suspended,
		RMAPEngine_MultipleTransactionScheme_NotSuspended,
		ByteOrderError,
		BadWordWidth,
		TimeoutNotEnabled,
		Timedout
	} RMAPExceptionStates;
	
	/** Constructor.
	 */
	RMAPException();
	
	/** Constructor.
	 */
	RMAPException(int newstatus);
	
	/** Setter for exception status.
	 * @param newstatus exception status
	 */
	void setStatus(int newstatus);
	
	/** Getter for exception status.
	 * @return exception status
	 */
	int getStatus();
	
	/** Dump this class.
	 */
	void dump();
};

#endif /*RMAPEXCEPTION_HH_*/
