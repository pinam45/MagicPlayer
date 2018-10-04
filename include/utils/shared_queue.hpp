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
#include <mutex>
#include <condition_variable>

template <typename T, typename Container = std::deque<T>>
class shared_queue
{
public:

	typedef T value_type;
	typedef typename Container::size_type size_type;

	shared_queue() noexcept(noexcept(Container{})) = default;

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
	std::mutex m_mutex;
	std::condition_variable m_cond;
};

template <typename T, typename Container>
typename shared_queue<T, Container>::value_type& shared_queue<T, Container>::front()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	while (m_queue.empty())
	{
		m_cond.wait(lock);
	}
	return m_queue.front();
}

template <typename T, typename Container>
void shared_queue<T, Container>::pop_front()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	while (m_queue.empty())
	{
		m_cond.wait(lock);
	}
	m_queue.pop_front();
}

template <typename T, typename Container>
void shared_queue<T, Container>::push_back(const typename shared_queue<T, Container>::value_type& item)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_queue.push_back(item);
	lock.unlock();
	m_cond.notify_one();
}

template <typename T, typename Container>
void shared_queue<T, Container>::push_back(typename shared_queue<T, Container>::value_type&& item)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_queue.push_back(std::forward<typename shared_queue<T, Container>::value_type>(item));
	lock.unlock();
	m_cond.notify_one();
}

template <typename T, typename Container>
typename shared_queue<T, Container>::size_type shared_queue<T, Container>::size()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	return m_queue.size();
}

template<typename T, typename Container>
bool shared_queue<T, Container>::empty() {
	std::unique_lock<std::mutex> lock(m_mutex);
	return m_queue.empty();
}

template<typename T, typename Container>
template<typename... Args>
void shared_queue<T, Container>::emplace_back(Args&&... args) {
	std::unique_lock<std::mutex> lock(m_mutex);
	m_queue.emplace_back(std::forward<Args>(args)...);
}


#endif //MAGICPLAYER_SHARED_QUEUE_HPP
