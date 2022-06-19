/*
	TCPClient.h
	Specter's TCP client rep using asio. Contains very basic ability to read and write from a tcp socket in a non-blocking, synchronous method.
	See asio docs for more details and examples on how to use.

	GWM -- April 2022

	Note: the write functionality has not been verified. Should be fine, but test before using.
*/
#include "TCPClient.h"

namespace Specter {

	TCPClient::TCPClient() :
		m_socket(m_context)
	{
		m_readBuffer.resize(s_readBufferSize);
	}

	TCPClient::TCPClient(const std::string& host, const std::string& port) :
		m_socket(m_context)
	{
		m_readBuffer.resize(s_readBufferSize);
		Connect(host, port);
	}

	TCPClient::~TCPClient()
	{
		Close();
	}

	void TCPClient::Connect(const std::string& host, const std::string& port)
	{
		try
		{
			asio::ip::tcp::resolver resolver(m_context);
			auto end_points = resolver.resolve(host, port);
			asio::connect(m_socket, end_points);
			m_socket.non_blocking(true); //Set the connection as non-blocking
		}
		catch (asio::system_error& e)
		{
			SPEC_ERROR("Unable to connect to remote port for TCPClient! Error Code: {0}", e.what());
		}
	}

	std::vector<char> TCPClient::Read()
	{
		asio::error_code code;
		size_t length = m_socket.read_some(asio::buffer(m_readBuffer, m_readBuffer.size()), code);
		if (code == asio::error::eof)
		{
			SPEC_WARN("Server has closed connection. Closing the TCPClient");
			Close();
		}
		else if (code && code != asio::error::would_block)
		{
			SPEC_ERROR("TCPClient::Read recieved unexpected error from host. Error message: {0}", code.message());
			SPEC_WARN("Closing the socket.");
			Close();
		}
		return std::vector<char>(m_readBuffer.begin(), m_readBuffer.begin()+length);
	}

	//untested, not currently used. 
	size_t TCPClient::Write(const std::vector<char>& data)
	{
		asio::error_code code;
		m_writeBuffer = data;
		size_t length = m_socket.write_some(asio::buffer(m_writeBuffer, m_writeBuffer.size()), code);
		if (code == asio::error::eof)
		{
			SPEC_WARN("Server has closed connection. Closing the TCPClient.");
			Close();
		}
		else if(code && code != asio::error::would_block)
		{
			SPEC_ERROR("TCPClient::Write recieved unexpected error from host. Error message: {0}", code.message());
			SPEC_WARN("Closing the socket");
			Close();
		}

		return length;
	}
}