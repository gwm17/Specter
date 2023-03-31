#include "RitualOnlineSource.h"
#include "../Caen/CompassHit.h"
#include "Specter/Utils/Functions.h"

namespace Specter {

	RitualOnlineSource::RitualOnlineSource(const std::string& hostname, const std::string& port, uint64_t coincidenceWindow) :
		DataSource(coincidenceWindow), m_client(hostname, port)
	{
		m_eventBuilder.SetSortFlag(true);
		m_validFlag = m_client.IsConnected();
	}

	RitualOnlineSource::~RitualOnlineSource()
	{
	}

	void RitualOnlineSource::ProcessData()
	{
		if (!m_client.IsConnected())
			m_validFlag = false;

		if (m_client.GetData(m_recievedMessage))
		{
			ReadMessage();
		}
	}

	void RitualOnlineSource::ReadMessage()
	{
		uint64_t nHits = m_recievedMessage.body.size() / m_recievedMessage.hitSize;
		uint64_t hitsRead = 0;
		uint8_t* bodyPointer = m_recievedMessage.body.data();
		CompassHit currentHit;
		SpecData convertedHit;
		while (hitsRead < nHits)
		{
			currentHit.board = *((uint16_t*)bodyPointer);
			bodyPointer += 2;
			currentHit.channel = *((uint16_t*)bodyPointer);
			bodyPointer += 2;
			currentHit.timestamp = *((uint64_t*)bodyPointer);
			bodyPointer += 8;
			if (Compass_IsEnergy(m_recievedMessage.dataType))
			{
				currentHit.energy = *((uint16_t*)bodyPointer);
				bodyPointer += 2;
			}
			if (Compass_IsEnergyCalibrated(m_recievedMessage.dataType))
			{
				currentHit.energyCalibrated = *((uint16_t*)bodyPointer);
				bodyPointer += 8;
			}
			if (Compass_IsEnergyShort(m_recievedMessage.dataType))
			{
				currentHit.energyShort = *((uint16_t*)bodyPointer);
				bodyPointer += 2;
			}
			currentHit.flags = *((uint32_t*)bodyPointer);
			bodyPointer += 4;

			hitsRead++;

			convertedHit.id = Utilities::GetBoardChannelUUID(currentHit.board, currentHit.channel);
			convertedHit.timestamp = currentHit.timestamp;
			convertedHit.longEnergy = currentHit.energy;
			convertedHit.shortEnergy = currentHit.energyShort;
			convertedHit.calEnergy = currentHit.energyCalibrated;

			m_eventBuilder.AddDatum(convertedHit);
		}
	}
}