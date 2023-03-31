#include "DYOnlineSource.h"

namespace Specter {

	DYOnlineSource::DYOnlineSource(const std::string& hostname, const std::string& port, uint64_t coincidenceWindow) :
		DataSource(coincidenceWindow), m_clientConnection(hostname, port)
	{
		m_eventBuilder.SetSortFlag(true);
		m_validFlag = m_clientConnection.IsConnected();
	}

	DYOnlineSource::~DYOnlineSource()
	{
	}

	void DYOnlineSource::ProcessData()
	{
		if (!m_clientConnection.IsConnected())
		{
			m_validFlag = false;
			return;
		}

		if (m_clientConnection.GetNextEvent(m_dyHit))
		{
			//Convert data from Daqromancy format to universal Specter format.
			m_datum.longEnergy = m_dyHit.energy;
			m_datum.shortEnergy = m_dyHit.energyShort;
			m_datum.timestamp = m_dyHit.timestamp;
			m_datum.id = Utilities::GetBoardChannelUUID(m_dyHit.board, m_dyHit.channel);
			m_eventBuilder.AddDatum(m_datum);
		}
	}
}