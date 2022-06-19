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
		PhysicsStartEvent(const std::string& loc, DataSource::SourceType type, uint64_t window, const std::string& port = "51489", bool sortFlag=false, uint16_t bitflags = 0,
						  int channels_per_board=16) :
			m_sourceLocation(loc), m_port(port), m_sourceType(type), m_coincidenceWindow(window), m_sortFlag(sortFlag), m_bitflags(bitflags), m_channels_per_board(channels_per_board)
		{}

		inline const std::string GetSourceLocation() const { return m_sourceLocation; }
        inline const std::string GetSourcePort() const { return m_port; }
		inline const DataSource::SourceType GetSourceType() const { return m_sourceType; }
		inline const uint64_t GetCoincidenceWindow() const { return m_coincidenceWindow; }
		inline const bool GetSortFlag() const { return m_sortFlag; }
		inline const int GetChannelsPerBoard() const { return m_channels_per_board; }
		inline const uint16_t GetBitFlags() const { return m_bitflags; }

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
		bool m_sortFlag;
		uint16_t m_bitflags;
		int m_channels_per_board;
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
