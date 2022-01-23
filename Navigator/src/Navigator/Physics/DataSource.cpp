#include "DataSource.h"
#include "CompassRun.h"
#include "CompassOnlineSource.h"

namespace Navigator {

	DataSource* CreateDataSource(const std::string& loc, DataSource::SourceType type)
	{
		switch(type)
		{
			case DataSource::SourceType::CompassOffline : return new CompassRun(loc);
			case DataSource::SourceType::CompassOnline : return new CompassOnlineSource(loc, "5346");
			case DataSource::SourceType::None : return nullptr;
		}
	}

	std::string ConvertDataSourceTypeToString(DataSource::SourceType type)
	{
		switch(type)
		{
			case DataSource::SourceType::None: return "None";
			case DataSource::SourceType::CompassOnline : return "CompassOnline";
			case DataSource::SourceType::CompassOffline : return "CompassOffline";
		}
	}
}