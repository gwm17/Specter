#include "CompassOnlineSource.h"

namespace Navigator {

	CompassOnlineSource::CompassOnlineSource(const std::string& hostname, const std::string& port) :
		DataSource(), m_bufferIter(nullptr), m_bufferEnd(nullptr), m_socket(m_socketContext)
	{
		m_buffer.resize(m_bufferSize);
		InitSocket(hostname, port);
	}

	CompassOnlineSource::~CompassOnlineSource() {}

	void CompassOnlineSource::InitSocket(const std::string& hostname, const std::string& port)
	{
		m_validFlag = false;
		try
		{
			asio::ip::tcp::resolver resolver(m_socketContext);
			auto end_points = resolver.resolve(hostname, port);
			asio::connect(m_socket, end_points);
		}
		catch (asio::system_error& error)
		{
			NAV_ERROR("Unable to connect to remote port for CompassOnlineSource! Error Code: {0}", error.what());
			return;
		}

		NAV_INFO("Successfully connected to host {0} on port {1}", hostname, port);
		m_validFlag = true;
	}

	const CompassHit& CompassOnlineSource::GetData()
	{
		if (!IsValid())
		{
			NAV_ERROR("Attempting to access invalid source at CompassOnlineSource!");
			m_currentHit = CompassHit();
			return m_currentHit;
		}
		else if (m_bufferIter == nullptr || m_bufferIter == m_bufferEnd)
		{
			FillBuffer();
		}
		
		if (m_bufferIter != m_bufferEnd)
		{
			GetHit();
			return m_currentHit;
		}
		else
		{
			m_currentHit = CompassHit();
			return m_currentHit;
		}
	}

	void CompassOnlineSource::FillBuffer()
	{
		asio::error_code code;

		size_t length = m_socket.read_some(asio::buffer(m_buffer), code);
		m_bufferEnd = m_buffer.data() + length;
		m_bufferIter = m_buffer.data();
		if (code == asio::error::eof)
		{
			NAV_WARN("CompassOnlineSource invalidated by host. Invalidating and detaching source.");
			m_validFlag = false;
		}
		else if (code)
		{
			NAV_ERROR("CompassOnlineSource recieved unexpected error from host. Error message: {0}", code.message());
			NAV_WARN("Invalidating and detaching source.");
			m_validFlag = false;
		}
	}

	void CompassOnlineSource::GetHit()
	{
		m_currentHit.board = *((uint16_t*)m_bufferIter);
		m_bufferIter += 2;
		m_currentHit.channel = *((uint16_t*)m_bufferIter);
		m_bufferIter += 2;
		m_currentHit.timestamp = *((uint64_t*)m_bufferIter);
		m_bufferIter += 8;
		m_currentHit.lgate = *((uint16_t*)m_bufferIter);
		m_bufferIter += 2;
		m_currentHit.sgate = *((uint16_t*)m_bufferIter);
		m_bufferIter += 2;
		m_currentHit.flags = *((uint32_t*)m_bufferIter);
		m_bufferIter += 4;
		m_currentHit.Ns = *((uint32_t*)m_bufferIter);
		m_bufferIter += 4;
	}

}