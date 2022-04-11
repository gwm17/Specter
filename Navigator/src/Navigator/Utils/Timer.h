#ifndef TIMER_H
#define TIMER_H

#include <chrono>

namespace Navigator {

	class Timer
	{
	public:
		Timer(const char* name) :
			m_name(name), m_stopped(false)
		{
			m_startTime = Clock::now();
		}

		~Timer()
		{
			if (!m_stopped)
				Stop();
		}


		void Stop()
		{
			auto stopTime = Clock::now();
			int64_t start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTime).time_since_epoch().count();
			int64_t stop = std::chrono::time_point_cast<std::chrono::microseconds>(stopTime).time_since_epoch().count();
			float duration = (stop - start)*0.001;
			m_stopped = true;

			NAV_INFO("{1} -- Duration: {0} ms", m_name, duration);
		}

	private:
		using Time = std::chrono::steady_clock::time_point;
		using Clock = std::chrono::steady_clock;

		const char* m_name;
		Time m_startTime;
		bool m_stopped;
	};

}

#endif