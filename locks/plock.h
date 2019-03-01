#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <atomic>


class PCLock{
	public:
				PCLock()=delete;
				PCLock(uint32_t tid);
				PCLock(const PCLock&)=delete;
				PCLock(PCLock&&)=delete;
				~PCLock(){}
				void lock();
				void unlock();
				void shared_lock();
				void shared_unlock();

	
	private:
				std::condition_variable cv;
				std::mutex mlock;
				std::atomic<int32_t> refcount{0};

};
