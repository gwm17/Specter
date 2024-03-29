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
		virtual std::vector<SpecEvent> GetEvents() override
		{
			auto temp = m_eventBuilder.GetReadyEvents();
			m_eventBuilder.ClearReadyEvents();
			return temp;
		}

		virtual const bool IsEventReady() const override { return m_eventBuilder.IsEventReady(); }

	private:
		DaqGrimoire::DYClient m_clientConnection;
		DaqGrimoire::DYListData m_dyHit;
	};
}

#endif