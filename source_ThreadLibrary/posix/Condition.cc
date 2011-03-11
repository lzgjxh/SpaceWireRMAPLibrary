#include "Condition.hh"

using namespace std;

/** ���� 
 * wait��signal�Ȃǂ̃��\�b�h�e����mutex.lock()���s���Ă��邪�A����͂����OK�B
 * pthread��condition�́Asignal/broadcast/wait/timedwait�̎��s�����Ƃ��āA
 * �R���X�g���N�^���ŗ^���Ă�������ϐ��p��mutex�����b�N����Ă��邱�Ƃ�v������̂ŁA
 * ���炩����lock����pthread_cond_wait�Ȃǂ��R�[������B���̌�Aptherad_cond_xxx��
 * �����ŁAmutex��unlock����Ă���҂��ɓ���̂ŁA���ۂɂ�wait()�Ȃǂ���������
 * �X���b�h����R�[���ł���B
 * 
 * ���̍�@(mutex�̃��b�N)��T-Kernel�̃C�x���g�t���O�ł͕s�v�Ȃ̂ŁA�ڐA�̂Ƃ���
 * ���ӂ��K�v�Bmutex�̃��b�N�̋L�q���c�����܂܂��ƁA�����X���b�h����wait()�Ȃǂ�
 * �ĂׂȂ��Ȃ邾���łȂ��Await()�̒���mutex�����b�N�����܂ܑ҂��ɓ����Ă��܂��̂ŁA
 * �ʂ̃X���b�h����signal()���悤�Ǝv���Ă��Amutex�̃��b�N���Ƃꂸ�ɉi�v�҂���
 * �����Ă��܂�(����ł�����ƃn�}����)�B
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
