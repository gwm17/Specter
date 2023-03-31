#ifndef RITUAL_CLIENT_H
#define RITUAL_CLIENT_H

#include "Specter/Utils/ThreadSafeQueue.h"
#include "Specter/Physics/SpecData.h"
#include <asio.hpp>
#include <thread>

namespace Specter {

	struct RitualMessage
	{
		uint64_t size; //Inclusive size of whole message 
		uint64_t hitSize; //Size of an individual CoMPASS data hit within the body
		uint16_t dataType; //CAEN header (CAEx)
		std::vector<uint8_t> body; //Data body
	};

	class RitualClient
	{
	public:
		RitualClient(const std::string& hostname, const std::string& port);
		~RitualClient();

		bool GetData(RitualMessage& reciever);

		void Connect(const std::string& hostname, const std::string& port);
		void Disconnect();

		const bool IsConnected() const { return m_socket.is_open(); }

		static constexpr uint64_t MinimumMessageSize() { return s_minimumMessageSize; }

	private:
		void ReadSize();
		void ReadHitSize();
		void ReadDataType();
		void ReadBody();
		void HandleTimeout(const asio::error_code& ec);

		asio::io_context m_context;
		asio::ip::tcp::socket m_socket;
		asio::steady_timer m_deadline;
		std::thread m_ioThread;

		RitualMessage m_tempMessage;
		ThreadSafeQueue<RitualMessage> m_queue;

		//All messages have a minimum size of size + hitSize + dataType
		static constexpr uint64_t s_minimumMessageSize = 64 + 64 + 16;
	};
}

#endif