/*
	DataSource.h
	Abstract data source class. In Specter a DataSource can be either an online (live) source or an offline (file) source. By default, 
	Specter has classes to handle CAEN CoMPASS data sources (files and online); other sources may be implemented as more use cases for Specter become
	apparent.

	GWM -- Feb 2022
*/
#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H

#include "Specter/Core/SpecCore.h"
#include "Specter/Physics/PhysicsEventBuilder.h"
#include "SpecData.h"

namespace Specter {

	class DataSource
	{
	public:
		enum class SourceType //Need to know what kind of sources are available.
		{
			None,
			CompassOnline,
			CompassOffline,
			DaqromancyOnline,
			DaqromancyOffline,
			CharonOnline
		};

		DataSource(uint64_t coincidenceWindow = 0) :
			m_validFlag(false), m_isEventReady(false), m_eventBuilder(coincidenceWindow)
		{
		}

		virtual ~DataSource() {};
		virtual void ProcessData() = 0;
		virtual const std::vector<SpecEvent>& GetEvents() = 0;
		inline bool IsValid() { return m_validFlag; }
		inline bool IsEventReady() { return m_isEventReady; }

	protected:
		bool m_validFlag;
		bool m_isEventReady;
		SpecData m_datum;
		PhysicsEventBuilder m_eventBuilder;
	};

	struct SourceArgs
	{
		DataSource::SourceType type = DataSource::SourceType::None;
		std::string location = "";
		std::string port = "";
		uint64_t coincidenceWindow = 0;
		uint16_t bitflags = 0;
	};

	DataSource* CreateDataSource(const SourceArgs& args);

	std::string ConvertDataSourceTypeToString(DataSource::SourceType type);
}

#endif
