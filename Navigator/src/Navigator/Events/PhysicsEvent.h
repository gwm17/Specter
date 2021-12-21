#ifndef PHYSICS_EVENT_H
#define PHYSICS_EVENT_H

#include "Event.h"

namespace Navigator {

	class PhysicsStartEvent : public Event
	{
	public:
		PhysicsStartEvent() {}

		std::string ToString() const override
		{
			return "Starting PhysicsEventBuilder";
		}

		EVENT_CATEGORY_SETUP(EventCategoryPhysics);
		EVENT_TYPE_SETUP(PhysicsStart);
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