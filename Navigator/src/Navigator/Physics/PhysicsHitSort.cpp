#include "PhysicsHitSort.h"

namespace Navigator {

	PhysicsHitSort::PhysicsHitSort(uint64_t window) :
		m_coincidenceWindow(window), m_startTime(0)
	{
	}

	PhysicsHitSort::~PhysicsHitSort() {}


	bool PhysicsHitSort::IsHitInWindow(const CompassHit& hit)
	{
		if(m_startTime == 0)
			return true;
		else if( (hit.timestamp - m_startTime) < m_coincidenceWindow)
			return true;
		else
			return false;
	}

	void PhysicsHitSort::AddHit(const CompassHit& hit)
	{
		if(m_event.size() == 0)
			m_startTime = hit.timestamp;

		m_event.emplace_back(hit);
	}

}