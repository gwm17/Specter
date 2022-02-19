#include "DataSource.h"
#include "Caen/CompassRun.h"
#include "Caen/CompassOnlineSource.h"

namespace Navigator {

	DataSource* CreateDataSource(const std::string& loc, const std::string& port, DataSource::SourceType type)
	{
		switch(type)
		{
			case DataSource::SourceType::CompassOffline : return new CompassRun(loc);
			case DataSource::SourceType::CompassOnline : return new CompassOnlineSource(loc, port);
			case DataSource::SourceType::None : return nullptr;
		}
		NAV_WARN("Invalid DataSourceType at CreateDataSource!");
		return nullptr;
	}

	std::string ConvertDataSourceTypeToString(DataSource::SourceType type)
	{
		switch(type)
		{
			case DataSource::SourceType::None: return "None";
			case DataSource::SourceType::CompassOnline : return "CompassOnline";
			case DataSource::SourceType::CompassOffline : return "CompassOffline";
		}

		return "None";
	}
}
