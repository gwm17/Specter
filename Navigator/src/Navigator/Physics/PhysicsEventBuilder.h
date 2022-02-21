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
		inline void ClearAll() 
		{
			m_event.clear();
			m_readyEvent.clear();
			m_eventStartTime = 0;
		}
		bool AddDatumToEvent(const NavData& datum);
		const NavEvent& GetReadyEvent() const;

	private:
		NavEvent m_event;
		NavEvent m_readyEvent;
		uint64_t m_coincWindow;
		uint64_t m_eventStartTime;
	};

}

#endif