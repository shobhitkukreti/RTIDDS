#ifndef THREAD_H_
#define THREAD_H_

#include <thread.h>
#include <pthread.h>
#include <iostream>

class STHREAD : public std::thread {

	public:
	STHREAD();
	void setPriority(uint32_t prio);
	private:
	sched_param sch_param;

};


