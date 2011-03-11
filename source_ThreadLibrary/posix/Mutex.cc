#include "Mutex.hh"

using namespace std;

Mutex::Mutex(){
	//�G���[�������s���Ă��Ȃ��̂Œ��ӂ��K�v
	pthread_mutexattr_init(&mutex_attr);
	pthread_mutexattr_settype(&mutex_attr,PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&mutex,&mutex_attr);
}

Mutex::~Mutex(){
	pthread_mutex_destroy(&mutex);
}

void Mutex::lock(){
	if(pthread_mutex_lock(&mutex)!=0){
		throw(SynchronousException("mutex lock exception"));
	}
}

void Mutex::unlock(){
	if(pthread_mutex_unlock(&mutex)!=0){
		throw(SynchronousException("mutex unlock exception"));
	}
}

pthread_mutex_t* Mutex::getPthread_Mutex_T(){
	return &mutex;
}
