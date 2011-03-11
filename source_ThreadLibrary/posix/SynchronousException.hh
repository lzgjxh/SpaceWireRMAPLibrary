#ifndef SYNCHRONOUSEXCEPTION_HH_
#define SYNCHRONOUSEXCEPTION_HH_

#include <string>
#include <iostream>
#include <exception>
using namespace std;

class SynchronousException : public exception {
private:
	string message;
public:
	SynchronousException(string str);
	~SynchronousException() throw ();
	void dump();
};

#endif /*SYNCHRONOUSEXCEPTION_HH_*/
