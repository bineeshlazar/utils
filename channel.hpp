/*
 * channel.hpp
 *
 *  Created on: 29-May-2019
 *      Author: bineesh
 */

#ifndef CHANNEL_HPP_
#define CHANNEL_HPP_


#include <list>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <memory>

template<class Item, class Alloc = std::allocator<Item>>
class channel {
private:
	std::list<Item, Alloc> m_queue;
	std::mutex m_mutex;
	std::condition_variable m_cv;
	bool m_closed;

	channel& operator=(const channel &) = delete;
	channel(const channel &) = delete;

public:
	channel() : m_closed(false){

	}

	void close()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_closed = true;
		m_cv.notify_all();
	}

	bool closed()
	{
		return m_closed;
	}

	void put(Item&& i)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		if(m_closed)
			throw std::runtime_error("put to closed channel");
		m_queue.push_back(std::forward<Item>(i));
		m_cv.notify_one();
	}

	template<class... Args>
	void emplace(Args&&... args)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		if(m_closed)
			throw std::runtime_error("put to closed channel");
		m_queue.emplace_back(std::forward<Args>(args)...);
		m_cv.notify_one();
	}

	bool get(Item &out, bool wait = true)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		if(wait)
			m_cv.wait(lock, [&](){ return m_closed || !m_queue.empty(); });
		if(m_queue.empty())
			return false;
		out = std::move(m_queue.front());
		m_queue.pop_front();
		return true;
	}
};

#endif /* CHANNEL_HPP_ */
