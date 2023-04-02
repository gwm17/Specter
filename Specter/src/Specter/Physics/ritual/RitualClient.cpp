#include "RitualClient.h"

namespace Specter {

	RitualClient::RitualClient(const std::string& hostname, const std::string& port) :
		m_socket(m_context), m_deadline(m_context)
	{
		Connect(hostname, port);
	}

	RitualClient::~RitualClient()
	{
		Disconnect();
	}

	bool RitualClient::GetData(RitualMessage& reciever)
	{
		if (m_queue.IsEmpty())
			return false;

		reciever = m_queue.Front();
		m_queue.PopFront();
		return true;
	}

	void RitualClient::Connect(const std::string& hostname, const std::string& port)
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
						SPEC_INFO("Connected RitualClient to {0}:{1}", endpoint.address(), endpoint.port());
						//Turn off our deadline timer
						m_deadline.cancel();
						ReadSize();
					}
					else
					{
						SPEC_WARN("Unable to connect to RitualClient {0}:{1}", hostname, port);
						m_socket.close();
					}
				}
			);

			m_deadline.async_wait(std::bind(&RitualClient::HandleTimeout, this, std::placeholders::_1));
			m_ioThread = std::thread([this]() { m_context.run(); });
		}
		catch (asio::system_error& e)
		{
			SPEC_WARN("Unable to connect RitualClient to {0}:{1}", hostname, port);
			return;
		}
	}

	void RitualClient::Disconnect()
	{
		SPEC_INFO("Disconnecting...");
		if (IsConnected())
		{
			asio::post(m_context, [this]() { m_socket.close(); });
		}
		SPEC_INFO("Disconnected...");

		SPEC_INFO("Stopping...");
		m_context.stop();
		if (m_ioThread.joinable())
			m_ioThread.join();
		SPEC_INFO("Stopped...");
	}

	void RitualClient::ReadSize()
	{
		asio::async_read(m_socket, asio::buffer(&m_tempMessage.size, sizeof(m_tempMessage.size)),
			[this](std::error_code ec, std::size_t size)
			{
				if (!ec)
				{
					m_tempMessage.body.resize(m_tempMessage.size - s_minimumMessageSize);
					ReadHitSize();
				}
			}
		);
	}

	void RitualClient::ReadHitSize()
	{
		asio::async_read(m_socket, asio::buffer(&m_tempMessage.hitSize, sizeof(m_tempMessage.hitSize)),
			[this](std::error_code ec, std::size_t size)
			{
				if (!ec)
				{
					ReadDataType();
				}
			}
		);
	}

	void RitualClient::ReadDataType()
	{
		asio::async_read(m_socket, asio::buffer(&m_tempMessage.dataType, sizeof(m_tempMessage.dataType)),
			[this](std::error_code ec, std::size_t size)
			{
				if (!ec)
				{
					ReadBody();
				}
			}
		);
	}

	void RitualClient::ReadBody()
	{
		asio::async_read(m_socket, asio::buffer(&m_tempMessage.body, m_tempMessage.body.size()),
			[this](std::error_code ec, std::size_t size)
			{
				if (!ec && size > 0)
				{
					m_queue.PushBack(m_tempMessage);
				}
				ReadSize();
			}
		);
	}

	void RitualClient::HandleTimeout(const asio::error_code& ec)
	{
		//If we stop the timer, don't do anything
		if (ec == asio::error::operation_aborted)
		{
			return;
		}
		//Check to make sure that deadline wasn't asychronously moved 
		if (m_deadline.expiry() <= asio::steady_timer::clock_type::now())
		{
			SPEC_WARN("RitualClient timedout at Connect!");
			m_socket.close();
			m_deadline.expires_at(asio::steady_timer::time_point::max());
		}
	}
}