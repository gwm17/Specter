#ifndef SPECTER_THREAD_SAFE_QUEUE_H
#define SPECTER_THREAD_SAFE_QUEUE_H

#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <condition_variable>

namespace Specter {

    template<typename T>
	class ThreadSafeQueue
	{
	public:
		ThreadSafeQueue() = default;
		ThreadSafeQueue(const ThreadSafeQueue&) = delete; //no copy
		~ThreadSafeQueue() { Clear(); }

		void PushBack(const T& data)
		{
			std::scoped_lock<std::mutex> guard(m_queueMutex);
			m_queue.push_back(data);

			std::scoped_lock<std::mutex> condGuard(m_conditionMutex);
			m_conditional.notify_one();
		}

		void PushFront(const T& data)
		{
			std::scoped_lock<std::mutex> guard(m_queueMutex);
			m_queue.push_front(data);

			std::scoped_lock<std::mutex> conditionGuard(m_conditionMutex);
			m_conditional.notify_one();
		}

		void PopBack()
		{
			std::scoped_lock<std::mutex> guard(m_queueMutex);
			m_queue.pop_back();
		}

		void PopFront()
		{
			std::scoped_lock<std::mutex> guard(m_queueMutex);
			m_queue.pop_front();
		}

		const T& Front()
		{
			std::scoped_lock<std::mutex> guard(m_queueMutex);
			return m_queue.front();
		}

		const T& Back()
		{
			std::scoped_lock<std::mutex> guard(m_queueMutex);
			return m_queue.back();
		}

		std::size_t Size()
		{
			std::scoped_lock<std::mutex> guard(m_queueMutex);
			return m_queue.size();
		}

		bool IsEmpty()
		{
			std::scoped_lock<std::mutex> guard(m_queueMutex);
			return m_queue.empty();
		}

		void Clear()
		{
			std::scoped_lock<std::mutex> guard(m_queueMutex);
			m_queue.clear();
		}

		//For iterator loops, need begin()/end() idiom

		typename std::deque<T>::iterator begin()
		{
			std::scoped_lock<std::mutex> guard(m_queueMutex);
			return m_queue.begin();
		}

		typename std::deque<T>::iterator end()
		{
			std::scoped_lock<std::mutex> guard(m_queueMutex);
			return m_queue.end();
		}

		void Wait()
		{
			while (IsEmpty() && !m_isForceWakeup)
			{
				std::unique_lock<std::mutex> guard(m_conditionMutex);
				m_conditional.wait(guard);
			}
		}

		void ForceWakeup()
		{
			m_isForceWakeup = true;

			std::unique_lock<std::mutex> guard(m_conditionMutex);
			m_conditional.notify_one();
		}

		void ResetWakeup()
		{
			m_isForceWakeup = false;
		}

	private:
		std::mutex m_queueMutex;
		std::deque<T> m_queue;

		std::mutex m_conditionMutex;
		std::condition_variable m_conditional;

		std::atomic<bool> m_isForceWakeup = false;
	};
    
}

#endif