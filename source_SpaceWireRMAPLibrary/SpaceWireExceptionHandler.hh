#ifndef SPACEWIREEXCEPTIONHANDLER_HH_
#define SPACEWIREEXCEPTIONHANDLER_HH_

#include "SpaceWireException.hh"

#include <iostream>
using namespace std;

class SpaceWireExceptionHandler {
public:
	SpaceWireExceptionHandler(void);
	virtual ~SpaceWireExceptionHandler(void) {}
public:
	virtual void handleSpaceWireException(SpaceWireException e) = 0;
	void dump();
};

#endif /*SPACEWIREEXCEPTIONHANDLER_HH_*/

/** History
 * 2008-10-07 file created (Takayuki Yuasa)
 */
