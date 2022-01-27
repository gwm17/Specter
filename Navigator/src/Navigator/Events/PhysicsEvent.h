#ifndef PHYSICS_EVENT_H
#define PHYSICS_EVENT_H

#include "Navigator/NavCore.h"
#include "Event.h"
#include "Navigator/Physics/DataSource.h"

namespace Navigator {

	class NAV_API PhysicsStartEvent : public Event
	{
	public:
		PhysicsStartEvent(const std::string& loc, DataSource::SourceType type, uint64_t window, const std::string& port = "51489") :
			m_sourceLocation(loc), m_port(port), m_sourceType(type), m_coincidenceWindow(window)
		{}

		inline std::string GetSourceLocation() { return m_sourceLocation; }
        inline std::string GetSourcePort() { return m_port; }
		inline DataSource::SourceType GetSourceType() { return m_sourceType; }
		inline uint64_t GetCoincidenceWindow() { return m_coincidenceWindow; }

		std::string ToString() const override
		{
			return "Starting PhysicsEventBuilder with DataSource of type {0} at location {1}" + m_sourceLocation + ConvertDataSourceTypeToString(m_sourceType);
		}

		EVENT_CATEGORY_SETUP(EventCategoryPhysics);
		EVENT_TYPE_SETUP(PhysicsStart);

	private:
		std::string m_sourceLocation;
        std::string m_port;
		DataSource::SourceType m_sourceType;
		uint64_t m_coincidenceWindow;
	};

	class NAV_API PhysicsStopEvent : public Event
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

	class NAV_API PhysicsParamEvent : public Event
	{
	public:
		PhysicsParamEvent() {}

		std::string ToString() const override
		{
			return "Updating Parameter lists!";
		}

		EVENT_CATEGORY_SETUP(EventCategoryPhysics);
		EVENT_TYPE_SETUP(PhysicsParam);
	};
}

#endif
