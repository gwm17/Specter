/*
	DataSource.h
	Abstract data source class. In Navigator a DataSource can be either an online (live) source or an offline (file) source. By default, 
	Navigator has classes to handle CAEN CoMPASS data sources (files and online); other sources may be implemented as more use cases for Navigator become
	apparent.

	GWM -- Feb 2022
*/
#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H

#include "Navigator/Core/NavCore.h"
#include "NavData.h"

namespace Navigator {
	
	class NAV_API DataSource
	{
	public:
		enum class SourceType //Need to know what kind of sources are available.
		{
			None,
			CompassOnline,
			CompassOffline
		};

		DataSource() :
			m_validFlag(false)
		{
		}

		virtual ~DataSource() {};
		virtual const NavData& GetData() = 0;
		inline bool IsValid() { return m_validFlag; }

	protected:
		bool m_validFlag;
		NavData m_datum;
	};

	NAV_API DataSource* CreateDataSource(const std::string& loc, const std::string& port, DataSource::SourceType type, uint16_t bitflags = 0);

	NAV_API std::string ConvertDataSourceTypeToString(DataSource::SourceType type);
}

#endif
