#ifndef DY_ONLINE_SOURCE_H
#define DY_ONLINE_SOURCE_H

#include "Specter/Physics/DataSource.h"
#include "NetIO/DYClient.h"

namespace Specter {

	class DYOnlineSource : public DataSource
	{
	public:
		DYOnlineSource(const std::string& hostname, const std::string& port, uint64_t coincidenceWindow);
		virtual ~DYOnlineSource();

		virtual void ProcessData() override;
		virtual const std::vector<SpecEvent>& GetEvents() override
		{
			m_isEventReady = false;
			return m_eventBuilder.GetReadyEvents();
		}

	private:
		DaqGrimoire::DYClient m_clientConnection;
		DaqGrimoire::DYListData m_dyHit;
	};
}

#endif