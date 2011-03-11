#ifndef RMAPEXCEPTIONHANDLER_HH_
#define RMAPEXCEPTIONHANDLER_HH_

#include "RMAPException.hh"
#include "SpaceWireExceptionHandler.hh"

#include <iostream>
using namespace std;

class RMAPExceptionHandler : public SpaceWireExceptionHandler {
public:
	RMAPExceptionHandler(void);
	virtual ~RMAPExceptionHandler(void) {}
public:
	virtual void handleRMAPException(RMAPException e) =0;
	virtual void handleSpaceWireException(SpaceWireException e){}
	void dump();
};


#endif /*RMAPEXCEPTIONHANDLER_HH_*/

/** History
 * 2008-10-07 file created (Takayuki Yuasa)
 */
