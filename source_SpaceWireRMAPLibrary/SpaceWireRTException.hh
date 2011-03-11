#ifndef SPACEWIRERTEXCEPTION_HH_
#define SPACEWIRERTEXCEPTION_HH_

#include <iostream>
using namespace std;

/** An exception status class passed when
 * an exception happens in SpaceWire-RT processes.
 */ 
class SpaceWireRTException {
public:
	static enum {
		NoException,
		InvalidCommunicationModel,
		InvalidQoSLevel,
		NotASpaceWireRTPacket
	} SpaceWireRTExceptionStates;
private:
	int status;
public:
	/** Constructor.
	 */
	SpaceWireRTxception();
	
	/** Constructor with status initialization.
	 * @param newstatus exception status
	 */
	SpaceWireRTException(int newstatus);
	
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

#endif /*SPACEWIRERTEXCEPTION_HH_*/
