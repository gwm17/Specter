#ifndef DY_ONLINE_SOURCE_H
#define DY_ONLINE_SOURCE_H

#include "Specter/Physics/DataSource.h"
#include "NetIO/DYClient.h"

namespace Specter {

	class DYOnlineSource : public DataSource
	{
	public:
		DYOnlineSource(const std::string& hostname, const std::string& port, int channelsPerBoard = 16);
		~DYOnlineSource();

		virtual const SpecData& GetData() override;

	private:
		DaqGrimoire::DYClient m_clientConnection;
		DaqGrimoire::DYListData m_dyHit;
		int m_channelsPerBoard;
	};
}

#endif