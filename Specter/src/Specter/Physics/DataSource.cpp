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
#include "nscldaq/CharonOnlineSource.h"

namespace Specter {

	//loc=either an ip address or a file location, port=address port, or unused in case of file
	DataSource* CreateDataSource(const std::string& location, const std::string& port, uint16_t header, DataSource::SourceType type, uint64_t coincidenceWindow)
	{
		SPEC_INFO("Here in create with type: {0} loc:{1} port:{2} ", ConvertDataSourceTypeToString(type), location, port);
		switch(type)
		{
			case DataSource::SourceType::CompassOffline : SPEC_INFO("CompassOff"); return new CompassRun(location, coincidenceWindow);
			case DataSource::SourceType::CompassOnline : SPEC_INFO("CompassOn"); return new CompassOnlineSource(location, port, header, coincidenceWindow);
			case DataSource::SourceType::DaqromancyOffline: SPEC_INFO("DaqOff"); return new DYFileSource(location, coincidenceWindow);
			case DataSource::SourceType::DaqromancyOnline: SPEC_INFO("DaqOn"); return new DYOnlineSource(location, port, coincidenceWindow);
			case DataSource::SourceType::CharonOnline: SPEC_INFO("CharOn");  return new CharonOnlineSource(location, port);
			case DataSource::SourceType::None: SPEC_INFO("None"); return nullptr;
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
			case DataSource::SourceType::CharonOnline: return "CharonOnline";
		}

		return "None";
	}
}
