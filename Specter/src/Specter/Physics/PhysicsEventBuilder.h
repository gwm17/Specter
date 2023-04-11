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
#ifndef PHYSICS_EVENT_BUILDER_H
#define PHYSICS_EVENT_BUILDER_H

#include "SpecData.h"

namespace Specter {

	class PhysicsEventBuilder
	{
	public:
		PhysicsEventBuilder();
		PhysicsEventBuilder(uint64_t windowSize);
		~PhysicsEventBuilder();
		void SetCoincidenceWindow(uint64_t windowSize) { m_coincWindow = windowSize; }
		void SetSortFlag(bool flag) { m_sortFlag = flag; }
		void ClearAll() // reset all internal structures
		{
			m_bufferIndex = 0;
			m_readyEvents.clear();
		}
		void ClearReadyEvents() { m_readyEvents.clear(); }
		void AddDatum(const SpecData& datum);
		bool IsEventReady() const { return !m_readyEvents.empty(); }
		std::vector<SpecEvent> GetReadyEvents() const;

	private:
		bool m_sortFlag;
		static constexpr int s_maxDataBuffer = 1000;
		std::array<SpecData, s_maxDataBuffer> m_dataBuffer;
		int m_bufferIndex;
		std::vector<SpecEvent> m_readyEvents;
		uint64_t m_coincWindow;

	};

}

#endif