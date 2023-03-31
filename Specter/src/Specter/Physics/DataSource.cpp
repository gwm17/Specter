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
#include "ritual/RitualOnlineSource.h"

namespace Specter {

	//loc=either an ip address or a file location, port=address port, or unused in case of file
	DataSource* CreateDataSource(const SourceArgs& args)
	{
		switch(args.type)
		{
			case DataSource::SourceType::CompassOffline: return new CompassRun(args.location, args.coincidenceWindow);
			case DataSource::SourceType::CompassOnline: return new CompassOnlineSource(args.location, args.port, args.bitflags, args.coincidenceWindow);
			case DataSource::SourceType::DaqromancyOffline: return new DYFileSource(args.location, args.coincidenceWindow);
			case DataSource::SourceType::DaqromancyOnline: return new DYOnlineSource(args.location, args.port, args.coincidenceWindow);
			case DataSource::SourceType::CharonOnline: return new CharonOnlineSource(args.location, args.port);
			case DataSource::SourceType::RitualOnline: return new RitualOnlineSource(args.location, args.port, args.coincidenceWindow);
			case DataSource::SourceType::None: return nullptr;
		}
		SPEC_WARN("Invalid DataSourceType at CreateDataSource!");
		return nullptr;
	}

	std::string ConvertDataSourceTypeToString(DataSource::SourceType type)
	{
		switch(type)
		{
			case DataSource::SourceType::None: return "None";
			case DataSource::SourceType::CompassOnline: return "CompassOnline";
			case DataSource::SourceType::CompassOffline: return "CompassOffline";
			case DataSource::SourceType::DaqromancyOffline: return "DaqromancyOffline";
			case DataSource::SourceType::DaqromancyOnline: return "DaqromancyOnline";
			case DataSource::SourceType::CharonOnline: return "CharonOnline";
			case DataSource::SourceType::RitualOnline: return "RitualOnline";
		}

		return "None";
	}
}
