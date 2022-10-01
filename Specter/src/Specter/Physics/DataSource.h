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
			DaqromancyOffline
		};

		DataSource() :
			m_validFlag(false)
		{
		}

		virtual ~DataSource() {};
		virtual const SpecData& GetData() = 0;
		inline bool IsValid() { return m_validFlag; }

	protected:
		bool m_validFlag;
		SpecData m_datum;
	};

	DataSource* CreateDataSource(const std::string& location, const std::string& port, uint16_t bitflags, int channels_per_board, DataSource::SourceType type);

	std::string ConvertDataSourceTypeToString(DataSource::SourceType type);
}

#endif
