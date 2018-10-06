/*****************************************************************************************
 *                                                                                       *
 * MIT License                                                                           *
 *                                                                                       *
 * Copyright (c) 2018 Maxime Pinard                                                      *
 *                                                                                       *
 * Permission is hereby granted, free of charge, to any person obtaining a copy          *
 * of this software and associated documentation files (the "Software"), to deal         *
 * in the Software without restriction, including without limitation the rights          *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell             *
 * copies of the Software, and to permit persons to whom the Software is                 *
 * furnished to do so, subject to the following conditions:                              *
 *                                                                                       *
 * The above copyright notice and this permission notice shall be included in all        *
 * copies or substantial portions of the Software.                                       *
 *                                                                                       *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR            *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,              *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE           *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER                *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,         *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE         *
 * SOFTWARE.                                                                             *
 *                                                                                       *
 *****************************************************************************************/

#ifndef MAGICPLAYER_SHARED_QUEUE_HPP
#define MAGICPLAYER_SHARED_QUEUE_HPP


#include <deque>
#include <atomic>
#include <mutex>
#include <condition_variable>

template<typename T, bool atomic_size = false, typename Container = std::deque<T>>
class shared_queue
{
public:

	typedef T value_type;
	typedef typename Container::size_type size_type;

	shared_queue() noexcept(noexcept(Container{}));

	value_type& front();
	void pop_front();

	void push_back(const value_type& item);
	void push_back(value_type&& item);

	template<typename... Args>
	void emplace_back(Args&&... args);

	size_type size();
	bool empty();

private:

	Container m_queue;
	std::atomic<size_type> m_size;
	std::mutex m_mutex;
	std::condition_variable m_cond;
};

template<typename T, bool atomic_size, typename Container>
shared_queue<T, atomic_size, Container>::shared_queue() noexcept(noexcept(Container{})) : m_size(m_queue.size()) {

}

template<typename T, bool atomic_size, typename Container>
typename shared_queue<T, atomic_size, Container>::value_type& shared_queue<T, atomic_size, Container>::front()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	while (m_queue.empty())
	{
		m_cond.wait(lock);
	}
	return m_queue.front();
}

template<typename T, bool atomic_size, typename Container>
void shared_queue<T, atomic_size, Container>::pop_front()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	while (m_queue.empty())
	{
		m_cond.wait(lock);
	}
	if constexpr (atomic_size){
		--m_size;
	}
	m_queue.pop_front();
}

template<typename T, bool atomic_size, typename Container>
void shared_queue<T, atomic_size, Container>::push_back(const typename shared_queue<T, atomic_size, Container>::value_type& item)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_queue.push_back(item);
	if constexpr (atomic_size){
		++m_size;
	}
	lock.unlock();
	m_cond.notify_one();
}

template<typename T, bool atomic_size, typename Container>
void shared_queue<T, atomic_size, Container>::push_back(typename shared_queue<T, atomic_size, Container>::value_type&& item)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_queue.push_back(std::forward<typename shared_queue<T, atomic_size, Container>::value_type>(item));
	if constexpr (atomic_size){
		++m_size;
	}
	lock.unlock();
	m_cond.notify_one();
}

template<typename T, bool atomic_size, typename Container>
template<typename... Args>
void shared_queue<T, atomic_size, Container>::emplace_back(Args&&... args) {
	std::unique_lock<std::mutex> lock(m_mutex);
	m_queue.emplace_back(std::forward<Args>(args)...);
	if constexpr (atomic_size){
		++m_size;
	}
	lock.unlock();
	m_cond.notify_one();
}

template<typename T, bool atomic_size, typename Container>
typename shared_queue<T, atomic_size, Container>::size_type shared_queue<T, atomic_size, Container>::size()
{
	if constexpr (atomic_size){
		return m_size;
	}
	else{
		std::unique_lock<std::mutex> lock(m_mutex);
		return m_queue.size();
	}
}

template<typename T, bool atomic_size, typename Container>
bool shared_queue<T, atomic_size, Container>::empty() {
	if constexpr (atomic_size){
		return m_size == 0;
	}
	else{
		std::unique_lock<std::mutex> lock(m_mutex);
		return m_queue.empty();
	}
}


#endif //MAGICPLAYER_SHARED_QUEUE_HPP
