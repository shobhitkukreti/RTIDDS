#include "thread.h"

STHREAD::STHREAD(){

}

void STHREAD::setPriority(uint32_t prio){

	sched_param sch;
	int policy;

	pthread_getschedparam(pthread_self(), &policy, &sch);
	sch.sched_priority = prio;
	pthread_setschedparam(pthread_self(), &policy, sch);
}
