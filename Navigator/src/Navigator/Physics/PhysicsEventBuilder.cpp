/*
	PhysicsEventBuilder.h
	Class for taking in raw NavData and converting into a NavEvent. NavEvent is just a std::vector of NavData, where
	the held NavData all falls within a time window called the coincidence window. As soon as a NavData is given that falls outside
	of this window, the current event is shifted to the ready event and the AddDatumToEvent function returns true. The ready event can
	then be retrieved. The hit that triggered the end of event then is used to start the new event. The current pattern is strongly associated
	with digital electronics concepts for nuclear data aquisition systems.

	GWM -- Feb 2022
*/
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
		if (m_eventStartTime == 0) //first ever event
		{
			m_eventStartTime = datum.timestamp;
			m_event.push_back(datum);
			return false;
		}
		else if (datum.timestamp - m_eventStartTime < m_coincWindow) //are we within active window
		{
			m_event.push_back(datum);
			return false;
		}
		else // found one that falls outside
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