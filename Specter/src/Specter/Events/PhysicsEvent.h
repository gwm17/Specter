/*
	KeyEvent.h
	Events related to physics processes. Again, based on @TheCherno's work.

	GWM -- Feb 2022

	Update to reflect new CAEN binary data format with headers to indicate data contents.

	GWM -- May 2022
*/
#ifndef PHYSICS_EVENT_H
#define PHYSICS_EVENT_H

#include "Event.h"
#include "Specter/Physics/DataSource.h"

namespace Specter {

	//When we start physics, need info for what kind of source we make
	class PhysicsStartEvent : public Event
	{
	public:
		//Bitflags is a final option for random crap needed for a source. Currently used for compass online to indicate header state.
		PhysicsStartEvent(const SourceArgs& args) :
			m_args(args)
		{}

		const SourceArgs& GetSourceArgs() const { return m_args; }

		std::string ToString() const override
		{
			return "Starting PhysicsEventBuilder with DataSource of type " + ConvertDataSourceTypeToString(m_args.type) + " at location " + m_args.location;
		}

		EVENT_CATEGORY_SETUP(EventCategoryPhysics);
		EVENT_TYPE_SETUP(PhysicsStart);

	private:
		SourceArgs m_args;
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

	//Unused. Exists as a potential path of upgrade
	class PhysicsParamEvent : public Event
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
