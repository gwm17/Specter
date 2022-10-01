/*
	DataSource.cpp
	Abstract data source class. In Specter a DataSource can be either an online (live) source or an offline (file) source. By default,
	Specter has classes to handle CAEN CoMPASS data sources (files and online); other sources may be implemented as more use cases for Specter become
	apparent.

	GWM -- Feb 2022
*/
#include "DataSource.h"
#include "Caen/CompassRun.h"
#include "Caen/CompassOnlineSource.h"
#include "Daqromancy/DYFileSource.h"
#include "Daqromancy/DYOnlineSource.h"

namespace Specter {

	//loc=either an ip address or a file location, port=address port, or unused in case of file
	DataSource* CreateDataSource(const std::string& location, const std::string& port, uint16_t header, DataSource::SourceType type)
	{
		switch(type)
		{
			case DataSource::SourceType::CompassOffline : return new CompassRun(location);
			case DataSource::SourceType::CompassOnline : return new CompassOnlineSource(location, port, header);
			case DataSource::SourceType::DaqromancyOffline: return new DYFileSource(location);
			case DataSource::SourceType::DaqromancyOnline: return new DYOnlineSource(location, port);
			case DataSource::SourceType::None : return nullptr;
		}
		SPEC_WARN("Invalid DataSourceType at CreateDataSource!");
		return nullptr;
	}

	std::string ConvertDataSourceTypeToString(DataSource::SourceType type)
	{
		switch(type)
		{
			case DataSource::SourceType::None: return "None";
			case DataSource::SourceType::CompassOnline : return "CompassOnline";
			case DataSource::SourceType::CompassOffline : return "CompassOffline";
			case DataSource::SourceType::DaqromancyOffline: return "DaqromancyOffline";
			case DataSource::SourceType::DaqromancyOnline: return "DaqromancyOnline";
		}

		return "None";
	}
}
