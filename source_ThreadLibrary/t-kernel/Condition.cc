#include "Condition.hh"

using namespace std;

Condition::Condition(){
	eventflag_attr.exinf=NULL;
	eventflag_attr.flgatr=TA_TFIFO | TA_WMUL;
	eventflag_attr.iflgptn=0;
	eventflagid=tk_cre_flg(&eventflag_attr);
	if(eventflagid<E_OK){
		throw(SynchronousException("Condition initialization exception"));
	}
}

Condition::~Condition(){
	if(eventflagid>=0){
		tk_del_flg(eventflagid);
	}
}

void Condition::wait(){
	UINT pointer;
	if(tk_wai_flg(eventflagid,0x01,TWF_ORW,&pointer,TMO_FEVR)<E_OK){
		throw(SynchronousException("Condition wait exception"));
	}
}

void Condition::wait(unsigned int millis){
	UINT pointer;
	int e=tk_wai_flg(eventflagid,0x01,TWF_ORW,&pointer,millis);
	if((e!=E_TMOUT) && e<E_OK){
		throw(SynchronousException("Condition wait exception"));
	}
}

void Condition::signal(){
	//T-Kernel EventFlag cannot perform "signal()" unlike POSIX.
	//Instead, we implement signal() as broadcast()
	broadcast();
}

void Condition::broadcast(){
	tk_set_flg(eventflagid,0x01);
	tk_clr_flg(eventflagid,0x00);
}