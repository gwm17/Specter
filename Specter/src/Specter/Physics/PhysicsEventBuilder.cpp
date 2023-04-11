/*
	PhysicsEventBuilder.h
	Class for taking in raw SpecData and converting into a SpecEvent. SpecEvent is just a std::vector of SpecData, where
	the held SpecData all falls within a time window called the coincidence window. As soon as a SpecData is given that falls outside
	of this window, the current event is shifted to the ready event vector. The ready event can then be retrieved. The hit that triggered the end of 
	event then is used to start the new event. The current pattern is strongly associated with digital electronics concepts 
	for nuclear data aquisition systems.

	GWM -- Feb 2022

	Added data time sorting, make event building model more compatible with different data sources.  -- GWM April 2023
*/
#include "PhysicsEventBuilder.h"

namespace Specter {

	PhysicsEventBuilder::PhysicsEventBuilder() :
		m_sortFlag(false), m_coincWindow(0), m_bufferIndex(0)
	{
	}

	PhysicsEventBuilder::PhysicsEventBuilder(uint64_t windowSize) :
		m_sortFlag(false), m_coincWindow(windowSize), m_bufferIndex(0)
	{
	}

	PhysicsEventBuilder::~PhysicsEventBuilder()
	{
	}

	void PhysicsEventBuilder::AddDatum(const SpecData& datum)
	{
		SPEC_PROFILE_FUNCTION();
		if (datum.timestamp == 0) //Ignore empty data (need a valid timestamp)
			return;

		m_dataBuffer[m_bufferIndex] = datum;
		m_bufferIndex++;
		if (m_bufferIndex < s_maxDataBuffer) //If we haven't filled the buffer keep going
			return;
		else if (m_sortFlag) //do time sorting if needed
			std::sort(m_dataBuffer.begin(), m_dataBuffer.end(), [](SpecData& i, SpecData& j) { return i.timestamp < j.timestamp; });
		
		//Generate our ready events
		m_readyEvents.clear();
		uint64_t eventStartTime = m_dataBuffer[0].timestamp;
		SpecEvent event;
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
		m_bufferIndex = 0; //Reset the buffer without reallocating
	}

	std::vector<SpecEvent> PhysicsEventBuilder::GetReadyEvents() const
	{
		return m_readyEvents;
	}

}