/** Sample Program for ThreadLibrary (Thread.cc, Condition.cc, and Mutex.cc)
 */

#include <iostream>
#include "Mutex.hh"

using namespace std;

int dump_frequency=1000000;

class Main {
private:
	Mutex mutex;
	int count;
	int count2;
public:
	Main(){
		count=0;
		count2=0;
	}
	void run(){
		while(true){
			Mutex m;
			m.lock();
			m.unlock();
			mutex.lock();
			count++;
			mutex.unlock();
			if(count==dump_frequency){
				cout << count2 << " " << count << endl;
				count=0;
				count2++;

			}
		}
	}
};

int main(int argn,char* argv[]){
	Main main;
	main.run();
	return 0;
}

/** History
 * 2008-12-15 file created (Takayuki Yuasa)
 */
