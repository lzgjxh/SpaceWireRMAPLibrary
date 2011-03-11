#include "SynchronousException.hh"

SynchronousException::SynchronousException(const string str) : message(str){}
SynchronousException::~SynchronousException() throw () {}

void SynchronousException::dump(){
	cout << "SynchronousException::dump()" << endl;
	cout << message << endl;
}
