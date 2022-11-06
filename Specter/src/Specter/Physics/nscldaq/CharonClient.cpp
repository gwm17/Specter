#include "CharonClient.h"

namespace Specter {

    CharonClient::CharonClient(const std::string& hostname, const std::string& port) :
        m_socket(m_context), m_deadline(m_context)
    {
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

			m_deadline.expires_after(std::chrono::seconds(30));
			asio::async_connect(m_socket, end_points,
				[this, hostname, port](std::error_code ec, asio::ip::tcp::endpoint endpoint)
				{
					if (!ec)
					{
            			SPEC_INFO("Connected CharonClient to {0}:{1}", endpoint.address(), endpoint.port());
						//Turn off our deadline timer
						m_deadline.cancel();
						ReadHeader();
					}
					else
					{
						SPEC_WARN("Unable to connect to CharonClient {0}:{1}" , hostname, port);
						m_socket.close();
					}
				}
			);

			m_deadline.async_wait(std::bind(&CharonClient::HandleTimeout, this, std::placeholders::_1));
			m_ioThread = std::thread([this]() { m_context.run(); });
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

	void CharonClient::HandleTimeout(const asio::error_code& ec)
	{
		//If we stop the timer, don't do anything
		if(ec == asio::error::operation_aborted)
		{
			return;
		}
		//Check to make sure that deadline wasn't asychronously moved 
		if(m_deadline.expiry() <= asio::steady_timer::clock_type::now())
		{
			SPEC_WARN("CharonClient timedout at Connect!");
			m_socket.close();
			m_deadline.expires_at(asio::steady_timer::time_point::max());
		}
	}
}