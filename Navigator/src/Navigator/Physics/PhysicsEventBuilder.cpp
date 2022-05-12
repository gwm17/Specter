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
		m_sortFlag(false), m_coincWindow(windowSize), m_bufferIndex(0)
	{
	}

	PhysicsEventBuilder::~PhysicsEventBuilder()
	{
	}

	bool PhysicsEventBuilder::AddDatum(const NavData& datum)
	{
		NAV_PROFILE_FUNCTION();
		if (datum.timestamp == 0) //Ignore empty data (need a valid timestamp)
			return false;

		m_dataBuffer[m_bufferIndex] = datum;
		m_bufferIndex++;
		if (m_bufferIndex < s_maxDataBuffer) //If we haven't filled the buffer keep going
			return false;
		else if (m_sortFlag)
			std::sort(m_dataBuffer.begin(), m_dataBuffer.end(), [](NavData& i, NavData& j) { return i.timestamp < j.timestamp; });
		
		//Generate our ready events
		m_readyEvents.clear();
		uint64_t eventStartTime = m_dataBuffer[0].timestamp;
		NavEvent event;
		event.push_back(m_dataBuffer[0]);
		for (auto& data : m_dataBuffer)
		{
			if (data.timestamp - eventStartTime < m_coincWindow) //are we within active window
			{
				event.push_back(data);
			}
			else // found one that falls outside
			{
				m_readyEvents.push_back(event);
				event.clear();
				eventStartTime = data.timestamp;
				event.push_back(data);
			}
		}
		m_bufferIndex = 0;
		return true;
	}

	const std::vector<NavEvent>& PhysicsEventBuilder::GetReadyEvents() const
	{
		return m_readyEvents;
	}

}