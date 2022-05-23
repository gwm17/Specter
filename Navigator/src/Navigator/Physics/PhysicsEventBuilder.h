/*
	PhysicsEventBuilder.h
	Class for taking in raw NavData and converting into a NavEvent. NavEvent is just a std::vector of NavData, where
	the held NavData all falls within a time window called the coincidence window. As soon as a NavData is given that falls outside
	of this window, the current event is shifted to the ready event and the AddDatumToEvent function returns true. The ready event can 
	then be retrieved. The hit that triggered the end of event then is used to start the new event. The current pattern is strongly associated
	with digital electronics concepts for nuclear data aquisition systems.

	GWM -- Feb 2022
*/
#ifndef PHYSICS_EVENT_BUILDER_H
#define PHYSICS_EVENT_BUILDER_H

#include "NavData.h"

namespace Navigator {

	class NAV_API PhysicsEventBuilder
	{
	public:
		PhysicsEventBuilder(uint64_t windowSize);
		~PhysicsEventBuilder();
		inline void SetCoincidenceWindow(uint64_t windowSize) { m_coincWindow = windowSize; }
		inline void SetSortFlag(bool flag) { m_sortFlag = flag; }
		inline void ClearAll() // reset all internal structures
		{
			m_bufferIndex = 0;
			m_readyEvents.clear();
		}
		bool AddDatum(const NavData& datum);
		const std::vector<NavEvent>& GetReadyEvents() const;

	private:
		bool m_sortFlag;
		static constexpr int s_maxDataBuffer = 1000;
		std::array<NavData, s_maxDataBuffer> m_dataBuffer;
		int m_bufferIndex;
		std::vector<NavEvent> m_readyEvents;
		uint64_t m_coincWindow;

	};

}

#endif