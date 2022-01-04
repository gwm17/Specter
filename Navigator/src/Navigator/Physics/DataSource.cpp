#include "DataSource.h"
#include "CompassRun.h"

namespace Navigator {

	DataSource* CreateDataSource(const std::string& loc, DataSource::SourceType type)
	{
		switch(type)
		{
			case DataSource::SourceType::CompassOffline : return new CompassRun(loc);
			case DataSource::SourceType::CompassOnline : return nullptr;
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