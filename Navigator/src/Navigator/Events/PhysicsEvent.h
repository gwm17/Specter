#ifndef PHYSICS_EVENT_H
#define PHYSICS_EVENT_H

#include "Event.h"
#include "Navigator/Physics/DataSource.h"

namespace Navigator {

	class PhysicsStartEvent : public Event
	{
	public:
		PhysicsStartEvent(const std::string& loc, DataSource::SourceType type) :
			m_sourceLocation(loc), m_sourceType(type)
		{}

		inline std::string GetSourceLocation() { return m_sourceLocation; }
		inline DataSource::SourceType GetSourceType() { return m_sourceType; }

		std::string ToString() const override
		{
			return "Starting PhysicsEventBuilder with DataSource of type {0} at location {1}" + m_sourceLocation + ConvertDataSourceTypeToString(m_sourceType);
		}

		EVENT_CATEGORY_SETUP(EventCategoryPhysics);
		EVENT_TYPE_SETUP(PhysicsStart);

	private:
		std::string m_sourceLocation;
		DataSource::SourceType m_sourceType;
	};

	class PhysicsStopEvent : public Event
	{
	public:
		PhysicsStopEvent() {}

		std::string ToString() const override
		{
			return "Stopping PhysicsEventBuilder";
		}

		EVENT_CATEGORY_SETUP(EventCategoryPhysics);
		EVENT_TYPE_SETUP(PhysicsStop);
	};

}

#endif