/*
	CompassOnlineSource.cpp
	A data source for online CAEN CoMPASS Data. Uses TCPClient to connect to the CoMPASS server. Data is then converted
	from the CAEN CoMPASS format to the native NavData format. Note that here we use syncrhonous since we
	need to know if the buffer is/was filled, however we use non-blocking since we don't want the entire process to hang on attempting a connection or waiting
	for data to come over the pipe. We handle the case of an un-filled buffer internally.

	IMPORTANT
	Specter wants a unqiue ID on each hit. To do this we use the idiom:
	id = board_number * nchannels_per_board + channel_number
	This requires two things: that the class variable m_nchannels_per_board be set to match your physical digitizers, and that ALL of your
	digitizers have the SAME number of channels. By default CompassRun assumes 16 channels per board, as this is what is used with the SE-SPS setup at FoxLab.
	If you use a different set of boards, CHANGE THIS VALUE! If you use mixed boards, you will need to invent a new id scheme altogether.

	ADDITIONALLY
	CoMPASS servers provide no stream side information on the state of a transfer (verified via communication w/ CAEN). That is: there are no headers or enders on the data transfers.
	This forces us to use the size of a single CoMPASS datum (CompassHit) to determine the state of a transfer. If the read buffer size is not a whole multiple of CompassHits, the data
	was determined to be fragmented. This has a huge drawback: in general there is no guarantee that the first byte of the transferred data is the first byte of a CompassHit. This means that
	Specter MUST be connected to the CoMPASS server BEFORE starting the aquisition. Otherwise the program could endup in a state of scrambled unpacking (also verified w/ CAEN).
	Maybe we can get them to change this? Headers reeaaally should exist for transfers like this.

	GWM -- April 2022
*/
#include "CompassOnlineSource.h"

namespace Specter {

	CompassOnlineSource::CompassOnlineSource(const std::string& hostname, const std::string& port, uint16_t header, int channels_per_board) :
		DataSource(), m_bufferIter(nullptr), m_bufferEnd(nullptr), m_header(header), m_nchannels_per_board(channels_per_board)
	{
		InitConnection(hostname, port);
	}

	CompassOnlineSource::~CompassOnlineSource() {}

	void CompassOnlineSource::InitConnection(const std::string& hostname, const std::string& port)
	{
		SPEC_PROFILE_FUNCTION();

		m_datasize = 16; //base size of CoMPASS data
		if (Compass_IsEnergy(m_header))
			m_datasize += 2;
		if (Compass_IsEnergyShort(m_header))
			m_datasize += 2;
		if (Compass_IsEnergyCalibrated(m_header))
			m_datasize += 8;
		if (Compass_IsWaves(m_header))
			SPEC_ERROR("Specter does not support reading CoMPASS wave data for an online source!");

		m_validFlag = false;
		m_connection.Connect(hostname, port);
		if (m_connection.IsOpen())
		{
			m_validFlag = true;
		}
	}

	const SpecData& CompassOnlineSource::GetData()
	{
		SPEC_PROFILE_FUNCTION();
		size_t range = m_bufferEnd - m_bufferIter; //how much buffer we have left
		if (!IsValid())
		{
			SPEC_ERROR("Attempting to access invalid source at CompassOnlineSource!");
			m_datum = SpecData();
			return m_datum;
		}
		else if (m_bufferIter == nullptr || range < m_datasize || m_bufferIter == m_bufferEnd) //If no buffer/buffer completely used/buffer fragmented fill 
		{
			FillBuffer();
		}
		
		if (m_bufferIter != m_bufferEnd && range >= m_datasize)//If buffer and enough data for a hit, get it
			GetHit();
		else
		{
			m_datum = SpecData();
			return m_datum;
		}

		m_datum.longEnergy = m_currentHit.energy;
		m_datum.shortEnergy = m_currentHit.energyShort;
		m_datum.calEnergy = m_currentHit.energyCalibrated;
		m_datum.timestamp = m_currentHit.timestamp;
		m_datum.id = m_currentHit.board * m_nchannels_per_board + m_currentHit.channel;

		return m_datum;
	}

	void CompassOnlineSource::FillBuffer()
	{
		SPEC_PROFILE_FUNCTION();
		if (!m_connection.IsOpen()) //Make sure connection is still cool
		{
			m_validFlag = false;
			return;
		}

		std::vector<char> recieved = m_connection.Read();
		//If we didn't finish the last buffer toss all of the stuff we used and then append the recieved data
		//Otherwise, copy over the recieved buffer. Note lack of vector::resize, vector::reserve. Intentional for performance.
		//The amount of copying/resizing is best handled by the std (according to multiple references)
		if (m_bufferIter != m_bufferEnd)
		{
			size_t pos = m_bufferEnd - m_bufferIter;
			m_currentBuffer.erase(m_currentBuffer.begin(), m_currentBuffer.begin() + (m_currentBuffer.size() - pos)); //remove used bytes
			m_currentBuffer.insert(m_currentBuffer.end(), recieved.begin(), recieved.end());
		}
		else
			m_currentBuffer = recieved;

		m_bufferIter = m_currentBuffer.data();
		m_bufferEnd = m_currentBuffer.data() + m_currentBuffer.size();
	}

	void CompassOnlineSource::ReadHeader()
	{

	}

	void CompassOnlineSource::GetHit()
	{
		SPEC_PROFILE_FUNCTION();
		m_currentHit.board = *((uint16_t*)m_bufferIter);
		m_bufferIter += 2;
		m_currentHit.channel = *((uint16_t*)m_bufferIter);
		m_bufferIter += 2;
		m_currentHit.timestamp = *((uint64_t*)m_bufferIter);
		m_bufferIter += 8;
		if (Compass_IsEnergy(m_header))
		{
			m_currentHit.energy = *((uint16_t*)m_bufferIter);
			m_bufferIter += 2;
		}
		if (Compass_IsEnergyCalibrated(m_header))
		{
			m_currentHit.energyCalibrated = *((uint16_t*)m_bufferIter);
			m_bufferIter += 8;
		}
		if (Compass_IsEnergyShort(m_header))
		{
			m_currentHit.energyShort = *((uint16_t*)m_bufferIter);
			m_bufferIter += 2;
		}
		m_currentHit.flags = *((uint32_t*)m_bufferIter);
		m_bufferIter += 4;
	}

}