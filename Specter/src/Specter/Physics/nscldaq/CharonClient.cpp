#include "CharonClient.h"

namespace Specter {

    CharonClient::CharonClient(const std::string& hostname, const std::string& port) :
        m_socket(m_context)
    {
		SPEC_INFO("here");
        Connect(hostname, port);
    }

    CharonClient::~CharonClient()
    {
        Disconnect();
    }

    bool CharonClient::GetNextEvent(std::vector<uint8_t>& event)
    {
        if(m_queue.IsEmpty())
            return false;

        event = m_queue.Front().body;
        m_queue.PopFront();
        return true;
    }

    void CharonClient::Connect(const std::string& hostname, const std::string& port)
    {
        try
		{
			asio::ip::tcp::resolver resolver(m_context);
			auto end_points = resolver.resolve(hostname, port);
			asio::async_connect(m_socket, end_points,
				[this](std::error_code ec, asio::ip::tcp::endpoint endpoint)
				{
					if (!ec)
					{
						ReadHeader();
					}
				}
			);

			m_ioThread = std::thread([this]() { m_context.run(); });
            SPEC_WARN("Connected CharonClient to {0}:{1}", hostname, port);
		}
		catch (asio::system_error& e)
		{
            SPEC_WARN("Unable to connect CharonClient to {0}:{1}", hostname, port);
			return;
		}
    }

    void CharonClient::Disconnect()
    {
        if (IsConnected())
		{
			asio::post(m_context, [this]() { m_socket.close(); });
		}

		m_context.stop();
		if (m_ioThread.joinable())
			m_ioThread.join();
    }

    void CharonClient::ReadHeader()
    {
        asio::async_read(m_socket, asio::buffer(&m_tempMessage.size, sizeof(m_tempMessage.size)),
			[this](std::error_code ec, std::size_t size)
			{
				if (!ec)
				{
					if (m_tempMessage.size > 0)
					{
						m_tempMessage.body.resize(m_tempMessage.size);
						ReadBody();
					}
					else
						ReadHeader();
				}
			}
		);
    }

    void CharonClient::ReadBody()
    {
        asio::async_read(m_socket, asio::buffer(m_tempMessage.body.data(), m_tempMessage.body.size()),
			[this](std::error_code ec, std::size_t size)
			{
				if (!ec)
				{
                    m_queue.PushBack(m_tempMessage);
				}
				ReadHeader();
			}
		);
    }
}