#ifndef PHYSICS_HIT_SORT_H
#define PHYSICS_HIT_SORT_H

#include "CompassHit.h"

namespace Navigator {

	class PhysicsHitSort
	{
	public:
		using RawPhysicsEvent = std::vector<CompassHit>;

		PhysicsHitSort(uint64_t window=1500000);
		~PhysicsHitSort();

		bool IsHitInWindow(const CompassHit& hit);
		void AddHit(const CompassHit& hit);

		inline void ClearRawPhysicsEvent() { m_event.clear(); }
		inline RawPhysicsEvent GetRawPhysicsEvent() { m_startTime = 0; return m_event; }

		inline void SetCoincidenceWindow(uint64_t window) { m_coincidenceWindow = window; }

	private:
		RawPhysicsEvent m_event;
		uint64_t m_coincidenceWindow;
		uint64_t m_startTime;
	};

}

#endif