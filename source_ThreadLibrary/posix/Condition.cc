#include "Condition.hh"

using namespace std;

/** メモ 
 * waitやsignalなどのメソッド各所でmutex.lock()を行っているが、これはこれでOK。
 * pthreadのconditionは、signal/broadcast/wait/timedwaitの実行条件として、
 * コンストラクタ部で与えている条件変数用のmutexがロックされていることを要求するので、
 * あらかじめlockしてpthread_cond_waitなどをコールする。その後、ptherad_cond_xxxの
 * 内部で、mutexがunlockされてから待ちに入るので、実際にはwait()などをいくつもの
 * スレッドからコールできる。
 * 
 * この作法(mutexのロック)はT-Kernelのイベントフラグでは不要なので、移植のときに
 * 注意が必要。mutexのロックの記述を残したままだと、複数スレッドからwait()などが
 * 呼べなくなるだけでなく、wait()の中でmutexをロックしたまま待ちに入ってしまうので、
 * 別のスレッドからsignal()しようと思っても、mutexのロックがとれずに永久待ちに
 * 入ってしまう(これでちょっとハマった)。
 */

Condition::Condition(){
	pthread_cond_init(&condition,NULL);
}

Condition::~Condition(){
	
}

void Condition::wait(){
	mutex.lock();
	pthread_cond_wait(&condition,mutex.getPthread_Mutex_T());
	mutex.unlock();
}

void Condition::wait(int millis){
	if ( millis <= 0 ) return;
	wait((unsigned int)millis);
}

void Condition::wait(unsigned int millis){
	struct timespec timeout;
	struct timeval tp;
	gettimeofday(&tp, NULL);

	timeout.tv_sec = (millis / 1000) + tp.tv_sec;
	timeout.tv_nsec = ((millis % 1000) * 1000000) + (tp.tv_usec * 1000);

	while (timeout.tv_nsec >= 1000000000) {
		timeout.tv_nsec -= 1000000000;
		timeout.tv_sec++;
	}

	mutex.lock();
	pthread_cond_timedwait(&condition,mutex.getPthread_Mutex_T(),&timeout);
	mutex.unlock();
}

void Condition::wait(double millis){
	struct timespec timeout;
	struct timeval tp;
	unsigned int sc, ns;

	if ( millis <= 0.0 ) return;

	sc = (unsigned int)(millis/1000);
	ns = (unsigned int)(1e9*(millis/1000 - sc));
	gettimeofday(&tp, NULL);
	timeout.tv_sec = sc + tp.tv_sec;
	timeout.tv_nsec = ns + (tp.tv_usec * 1000);

	while (timeout.tv_nsec >= 1000000000) {
		timeout.tv_nsec -= 1000000000;
		timeout.tv_sec++;
	}

	mutex.lock();
	pthread_cond_timedwait(&condition,mutex.getPthread_Mutex_T(),&timeout);
	mutex.unlock();
}

void Condition::signal(){
	mutex.lock();
	pthread_cond_signal(&condition);
	mutex.unlock();
}

void Condition::broadcast(){
	mutex.lock();
	pthread_cond_broadcast(&condition);
	mutex.unlock();
}

Mutex* Condition::getConditionMutex(){
	return &mutex;
}
