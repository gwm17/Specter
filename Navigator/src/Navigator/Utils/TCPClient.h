/*
	TCPClient.h
	Navigator's TCP client rep using asio. Contains very basic ability to read and write from a tcp socket in a non-blocking, synchronous method.
	See asio docs for more details and examples on how to use.

	GWM -- April 2022

	Note: the write functionality has not been verified. Should be fine, but test before using.
*/
#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <asio.hpp>

namespace Navigator {

	class TCPClient
	{
	public:
		TCPClient();
		TCPClient(const std::string& host, const std::string& port);
		~TCPClient();

		void Connect(const std::string& host, const std::string& port);
		std::vector<char> Read();
		size_t Write(const std::vector<char>& data);
		inline void Close() { if(IsOpen()) m_socket.close(); }
		inline bool IsOpen() { return m_socket.is_open(); }

	private:

		std::vector<char> m_readBuffer;
		std::vector<char> m_writeBuffer;

		static constexpr size_t s_readBufferSize = 24000; // reserve some space for the read buffer
		asio::io_context m_context;
		asio::ip::tcp::socket m_socket;
	};
}

#endif