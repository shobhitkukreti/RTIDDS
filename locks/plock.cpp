// BUSY WAIT READER WRITER LOCK

#include "plock.h"

PCLock::PCLock(uint32_t tid)
{
				// setup thread id with priority support for PCP
}

void PCLock::lock(){
				int32_t memVal=0;
				
				do{
								memVal=0;	
				} while(!refcount.compare_exchange_weak(memVal,-1,std::memory_order_acquire));
}

void PCLock::unlock(){
				refcount.store(0, std::memory_order_release);
}

void PCLock::shared_lock(){
				int32_t memVal;
				do{
								do{
												memVal=refcount.load(std::memory_order_release);

								}while(memVal==-1); // while exclusive lock is released
				} while(!refcount.compare_exchange_weak(memVal, memVal+1, std::memory_order_acquire));
}

void PCLock::shared_unlock(){
				refcount.fetch_sub(1, std::memory_order_release);

}
