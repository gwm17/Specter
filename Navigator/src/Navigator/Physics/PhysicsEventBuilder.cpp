#include "PhysicsEventBuilder.h"

namespace Navigator {

	PhysicsEventBuilder::PhysicsEventBuilder(uint64_t windowSize) :
		m_coincWindow(windowSize), m_eventStartTime(0)
	{
	}

	PhysicsEventBuilder::~PhysicsEventBuilder()
	{
	}

	bool PhysicsEventBuilder::AddDatumToEvent(const NavData& datum)
	{
		if (m_eventStartTime == 0)
		{
			m_eventStartTime = datum.timestamp;
			m_event.push_back(datum);
			return false;
		}
		else if (datum.timestamp - m_eventStartTime < m_coincWindow)
		{
			m_event.push_back(datum);
			return false;
		}
		else
		{
			m_readyEvent = m_event;
			m_event.clear();
			m_eventStartTime = datum.timestamp;
			m_event.push_back(datum);
			return true;
		}
	}

	const NavEvent& PhysicsEventBuilder::GetReadyEvent() const
	{
		return m_readyEvent;
	}

}