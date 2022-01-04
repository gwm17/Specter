#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H

#include "CompassHit.h"

namespace Navigator {

	class DataSource
	{
	public:
		enum class SourceType
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
		virtual CompassHit GetData() = 0;
		inline bool IsValid() { return m_validFlag; }

	protected:
		bool m_validFlag;
	};

	DataSource* CreateDataSource(const std::string& loc, DataSource::SourceType type);

	std::string ConvertDataSourceTypeToString(DataSource::SourceType type);
}

#endif