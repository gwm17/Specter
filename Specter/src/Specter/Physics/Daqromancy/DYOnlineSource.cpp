#include "DYOnlineSource.h"

namespace Specter {

	DYOnlineSource::DYOnlineSource(const std::string& hostname, const std::string& port, int channelsPerBoard) :
		DataSource(), m_clientConnection(hostname, port), m_channelsPerBoard(channelsPerBoard)
	{
		m_validFlag = m_clientConnection.IsConnected();
	}

	DYOnlineSource::~DYOnlineSource()
	{
	}

	const SpecData& DYOnlineSource::GetData()
	{
		if (!m_clientConnection.IsConnected())
		{
			m_validFlag = false;
			m_datum = SpecData();
			return m_datum;
		}

		if (m_clientConnection.GetNextEvent(m_dyHit))
		{
			//Convert data from Daqromancy format to universal Specter format.
			m_datum.longEnergy = m_dyHit.energy;
			m_datum.shortEnergy = m_dyHit.energyShort;
			m_datum.timestamp = m_dyHit.timestamp;
			m_datum.id = m_dyHit.board * m_channelsPerBoard + m_dyHit.channel;
		}
		else
		{
			m_datum = SpecData();
		}

		return m_datum;
	}
}