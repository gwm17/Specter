#ifndef TIMESTEP_H
#define TIMESTEP_H

namespace Specter {

	class Timestep
	{
	public:
		Timestep(float time=0.0f) :
			m_time(time)
		{
		}

		void SetTime(float time) { m_time = time; }

		operator float() const { return m_time; }

		float GetElapsedSeconds() const { return m_time; }
		float GetElapsedMilliseconds() const { return m_time * 1000.0f; }
	private:
		float m_time;
	};
}

#endif