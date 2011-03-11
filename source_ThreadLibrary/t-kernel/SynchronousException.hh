#ifndef SYNCHRONOUSEXCEPTION_HH_
#define SYNCHRONOUSEXCEPTION_HH_

#include <string>
#include <iostream>
#include <exception>
using namespace std;

/** A class which is thrown when an exception
 * occurs in ThreadLibary.
 */
class SynchronousException : public exception {
private:
	string message;
public:
	/** Constructo with exception message.
	 * @param str message to be dumped
	 */
	SynchronousException(string str);

	/** Destructor.
	 */
	~SynchronousException() throw ();

	/** Dumps the information on a certain instance.
	 */
	void dump();
};

#endif /*SYNCHRONOUSEXCEPTION_HH_*/
