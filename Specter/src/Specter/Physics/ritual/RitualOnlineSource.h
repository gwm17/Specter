#ifndef RITUAL_ONLINE_SOURCE_H
#define RITUAL_ONLINE_SOURCE_H

#include "Specter/Physics/DataSource.h"
#include "RitualClient.h"

namespace Specter {

	class RitualOnlineSource : public DataSource
	{
	public:
		RitualOnlineSource(const std::string& hostname, const std::string& port, uint64_t coincidenceWindow);
		virtual ~RitualOnlineSource();

		virtual void ProcessData() override;
		virtual std::vector<SpecEvent> GetEvents() override
		{
			auto temp = m_eventBuilder.GetReadyEvents();
			m_eventBuilder.ClearReadyEvents();
			return temp;
		}
		virtual const bool IsEventReady() const override { return m_eventBuilder.IsEventReady(); }

	private:
		void ReadMessage();
		
		RitualClient m_client;
		RitualMessage m_recievedMessage;
	};
}

#endif