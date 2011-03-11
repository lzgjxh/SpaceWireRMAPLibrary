#include "Mutex.hh"

using namespace std;

Mutex::Mutex(){
	mutex_attr.exinf=NULL;
	mutex_attr.mtxatr=TA_TFIFO;
	mutexid=tk_cre_mtx(&mutex_attr);
	if(mutexid<E_OK){
		throw(SynchronousException("Mutex initialization exception"));
	}
}

Mutex::~Mutex(){
	if(mutexid>=E_OK){
		tk_del_mtx(mutexid);
	}
}

void Mutex::lock(){
	if(tk_loc_mtx(mutexid,TMO_FEVR)<E_OK){
		throw(SynchronousException("mutex lock exception"));
	}
}

void Mutex::unlock(){
	if(tk_unl_mtx(mutexid)<E_OK){
		throw(SynchronousException("mutex unlock exception"));
	}
}

ID Mutex::getMutexID(){
	return mutexid;
}