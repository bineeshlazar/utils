/*
 * threadpool.hpp
 *
 *  Created on: 05-Jun-2019
 *      Author: bineesh
 *      Inspired from : github.com/progschj/ThreadPool
 */



#ifndef THREADPOOL_HPP_
#define THREADPOOL_HPP_

#include <cstddef>
#include <vector>
#include <thread>
#include <list>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <iostream>

#include "external/MemoryPool/C-11/MemoryPool.h"

namespace utils {

class threadpool {
	typedef std::function<void()> taskfunc;

	std::vector<std::thread> m_workers;
	std::list<taskfunc, MemoryPool<taskfunc>> m_tasks;
	std::mutex m_taskq_mtx;
	std::condition_variable m_cv;
	bool m_stopped;

	threadpool(const threadpool &) = delete;
	threadpool& operator=(const threadpool &) = delete;

public:
	threadpool(std::size_t size ):
		m_stopped(false)
	{
		for (std::size_t i = 0; i < size; i++) {
			m_workers.emplace_back([this](){
				for(;;){
					taskfunc task;
					{
						std::unique_lock<std::mutex> lock(this->m_taskq_mtx);

						this->m_cv.wait(lock, [this](){
							return this->m_stopped || !this->m_tasks.empty();
						});

						if(this->m_tasks.empty() && this->m_stopped){
							return;
						}

						task = std::move(this->m_tasks.front());
						this->m_tasks.pop_front();
					}

					try {
						task();
					} catch (std::exception &e) {
						std::cout <<  "Task failed\n" << e.what() << std::endl;
					}
				}
			});
		}
	}

	// Post a task to pool. arguments are passed by values
	// Make sure the arguments match.Otherwise bind will throw cryptic errors
	template<class F, class... Args>
	void post(F&& f, Args&&... args){
		using namespace std;
		{
			unique_lock<mutex> lock(m_taskq_mtx);
			if (m_stopped) throw runtime_error("post on stopped threadpool");
			m_tasks.emplace_back(bind(forward<F>(f), forward<Args>(args)...));
		}
		m_cv.notify_one();
	}

	~threadpool()
	{
		{
			std::unique_lock<std::mutex> lock(m_taskq_mtx);
			m_stopped = true;
		}
		m_cv.notify_all();

		for (auto &worker : m_workers) {
			worker.join();
		}
	}
};

}
#endif /* THREADPOOL_HPP_ */
