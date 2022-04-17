/*
	TestServerLayer.h
	This set of classes represents a test layer containing a TCP server and client connection class. This is only to be used as a way to debug
	online data sources. You can have this server post data to a port on a localhost and then have the analysis thread connect to that port and analyze the
	imitated data. Default setup is for an OnlineCompassSource, but of course this could be easily modified for your test of choice. For more details,
	see the asio Tutorials, specifically the asynchronous daytime server.

	Here we use async methods, as we do not want the whole project to be hanging on creating a succesfull client/server connection. (Also cause it was fun
	to learn)

	GWM -- April 2022

	NOTE: There are NO security features on this server/connection. Please use with care on a protected network/firewalled machine.
*/
#include "TestServerLayer.h"

namespace Navigator {

	/* TCPConnection */

	TCPConnection::TCPConnection(asio::io_context& context) :
		m_socket(context), m_buffer(36)
	{
	}

	//This function is kinda misnamed in our scheme. Should be Write to make more clear.
	void TCPConnection::Start()
	{
		CreateBinaryBuffer(); //Generate Buffer
		//CreateBinaryBufferFragmented(); //Generate fragmented buffer
		//Actually write the buffer to the socket. Use std::bind to set a callback function for error handling or any other server side actions
		asio::async_write(m_socket, asio::buffer(m_buffer, m_buffer.size()),
			std::bind(&TCPConnection::HandleWrite, this, std::placeholders::_1, std::placeholders::_2)); 
	}

	//Server-side connection actions upon attempting write, only on for debugging
	void TCPConnection::HandleWrite(const asio::error_code& ec, size_t bytes)
	{
		/*
		static int i = 0;
		if(!ec)
			NAV_INFO("Number written: {0}", ++i);
		*/
		//NAV_INFO("Writer result: Asio Error -- {0} Amount transferred={1}", ec.message(), bytes);
	}

	/*
		Create C-style binary buffer from the data struct. This is to mimic the raw data source,
		which will have no padding, or any other normal struct related features (and note that the intrisic
		ordering from compass ensures that padding would be placed)
	*/
	void TCPConnection::CreateBinaryBuffer()
	{
		m_hit.board = 8;
		m_hit.channel = 1;
		m_hit.timestamp = m_hit.timestamp + s_timestep;
		m_hit.lgate = 512;
		m_hit.sgate = 0;
		m_hit.flags = 0;
		m_hit.Ns = 0;


		char* data_pointer;
		int buffer_position = 0;
		data_pointer = (char*)&m_hit.board;
		for (int i = 0; i < 2; i++)
		{
			m_buffer[buffer_position] = *(data_pointer + i);
			buffer_position++;
		}
		data_pointer = (char*)&m_hit.channel;
		for (int i = 0; i < 2; i++)
		{
			m_buffer[buffer_position] = *(data_pointer + i);
			buffer_position++;
		}
		data_pointer = (char*)&m_hit.timestamp;
		for (int i = 0; i < 8; i++)
		{
			m_buffer[buffer_position] = *(data_pointer + i);
			buffer_position++;
		}
		data_pointer = (char*)&m_hit.lgate;
		for (int i = 0; i < 2; i++)
		{
			m_buffer[buffer_position] = *(data_pointer + i);
			buffer_position++;
		}
		data_pointer = (char*)&m_hit.sgate;
		for (int i = 0; i < 2; i++)
		{
			m_buffer[buffer_position] = *(data_pointer + i);
			buffer_position++;
		}
		data_pointer = (char*)&m_hit.flags;
		for (int i = 0; i < 4; i++)
		{
			m_buffer[buffer_position] = *(data_pointer + i);
			buffer_position++;
		}
		data_pointer = (char*)&m_hit.Ns;
		for (int i = 0; i < 4; i++)
		{
			m_buffer[buffer_position] = *(data_pointer + i);
			buffer_position++;
		}
	}

	/*
		Create C-style binary buffer from the data struct. This is to mimic the raw data source,
		which will have no padding, or any other normal struct related features (and note that the intrisic
		ordering from compass ensures that padding would be placed). Here we also fragment a hit (split over 2 buffers)
		to test ability to handle real life conditions

		Note -- as implemented highlights one issue with CAEN CoMPASS sources. No headers are left in the stream,
		so one must be CERTAIN to attach Navigator to the socket before running, otherwise fragments could lead to 
		scrambled unpacking order. (i.e. sometimes this will work for the test and sometimes it won't)
	*/
	void TCPConnection::CreateBinaryBufferFragmented()
	{
		static std::atomic<bool> even = true;
		m_hit.board = 8;
		m_hit.channel = 1;
		m_hit.timestamp = m_hit.timestamp + s_timestep;
		m_hit.lgate = 512;
		m_hit.sgate = 0;
		m_hit.flags = 0;
		m_hit.Ns = 0;

		
		char* data_pointer;
		int buffer_position = 0;
		if (even)
		{
			data_pointer = (char*)&m_hit.board;
			for (int i = 0; i < 2; i++)
			{
				m_buffer[buffer_position] = *(data_pointer + i);
				buffer_position++;
			}
			data_pointer = (char*)&m_hit.channel;
			for (int i = 0; i < 2; i++)
			{
				m_buffer[buffer_position] = *(data_pointer + i);
				buffer_position++;
			}
			data_pointer = (char*)&m_hit.timestamp;
			for (int i = 0; i < 8; i++)
			{
				m_buffer[buffer_position] = *(data_pointer + i);
				buffer_position++;
			}
			data_pointer = (char*)&m_hit.lgate;
			for (int i = 0; i < 2; i++)
			{
				m_buffer[buffer_position] = *(data_pointer + i);
				buffer_position++;
			}
			data_pointer = (char*)&m_hit.sgate;
			for (int i = 0; i < 2; i++)
			{
				m_buffer[buffer_position] = *(data_pointer + i);
				buffer_position++;
			}
			data_pointer = (char*)&m_hit.flags;
			for (int i = 0; i < 4; i++)
			{
				m_buffer[buffer_position] = *(data_pointer + i);
				buffer_position++;
			}
			data_pointer = (char*)&m_hit.Ns;
			for (int i = 0; i < 4; i++)
			{
				m_buffer[buffer_position] = *(data_pointer + i);
				buffer_position++;
			}
			data_pointer = (char*)&m_hit.board;
			for (int i = 0; i < 2; i++)
			{
				m_buffer[buffer_position] = *(data_pointer + i);
				buffer_position++;
			}
			data_pointer = (char*)&m_hit.channel;
			for (int i = 0; i < 2; i++)
			{
				m_buffer[buffer_position] = *(data_pointer + i);
				buffer_position++;
			}
			m_hit.timestamp += s_timestep;
			data_pointer = (char*)&m_hit.timestamp;
			for (int i = 0; i < 8; i++)
			{
				m_buffer[buffer_position] = *(data_pointer + i);
				buffer_position++;
			}
			even = false;
		}
		else
		{
			data_pointer = (char*)&m_hit.lgate;
			for (int i = 0; i < 2; i++)
			{
				m_buffer[buffer_position] = *(data_pointer + i);
				buffer_position++;
			}
			data_pointer = (char*)&m_hit.sgate;
			for (int i = 0; i < 2; i++)
			{
				m_buffer[buffer_position] = *(data_pointer + i);
				buffer_position++;
			}
			data_pointer = (char*)&m_hit.flags;
			for (int i = 0; i < 4; i++)
			{
				m_buffer[buffer_position] = *(data_pointer + i);
				buffer_position++;
			}
			data_pointer = (char*)&m_hit.Ns;
			for (int i = 0; i < 4; i++)
			{
				m_buffer[buffer_position] = *(data_pointer + i);
				buffer_position++;
			}
			data_pointer = (char*)&m_hit.board;
			for (int i = 0; i < 2; i++)
			{
				m_buffer[buffer_position] = *(data_pointer + i);
				buffer_position++;
			}
			data_pointer = (char*)&m_hit.channel;
			for (int i = 0; i < 2; i++)
			{
				m_buffer[buffer_position] = *(data_pointer + i);
				buffer_position++;
			}
			data_pointer = (char*)&m_hit.timestamp;
			for (int i = 0; i < 8; i++)
			{
				m_buffer[buffer_position] = *(data_pointer + i);
				buffer_position++;
			}
			data_pointer = (char*)&m_hit.lgate;
			for (int i = 0; i < 2; i++)
			{
				m_buffer[buffer_position] = *(data_pointer + i);
				buffer_position++;
			}
			data_pointer = (char*)&m_hit.sgate;
			for (int i = 0; i < 2; i++)
			{
				m_buffer[buffer_position] = *(data_pointer + i);
				buffer_position++;
			}
			data_pointer = (char*)&m_hit.flags;
			for (int i = 0; i < 4; i++)
			{
				m_buffer[buffer_position] = *(data_pointer + i);
				buffer_position++;
			}
			data_pointer = (char*)&m_hit.Ns;
			for (int i = 0; i < 4; i++)
			{
				m_buffer[buffer_position] = *(data_pointer + i);
				buffer_position++;
			}
			even = true;
		}
	}

	/* TCPServer */

	TCPServer::TCPServer(asio::io_context& context) :
		m_contextRef(context), m_acceptor(context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 51489)), m_isAccepted(false)
	{
	}

	//Attempt to create a connection
	std::shared_ptr<TCPConnection> TCPServer::StartAccept()
	{
		std::shared_ptr<TCPConnection> new_connection = TCPConnection::CreateConnection(m_contextRef);

		m_acceptor.async_accept(new_connection->Socket(), std::bind(&TCPServer::HandleAccept, this, new_connection, std::placeholders::_1));

		return new_connection;
	}

	//If connection successful, attempt to write as a test.
	void TCPServer::HandleAccept(std::shared_ptr<TCPConnection> connection, const asio::error_code& ec)
	{
		if (!ec)
		{
			m_isAccepted = true;
			connection->Start();
		}
		else
		{
			m_isAccepted = false;
			NAV_INFO("TCPServer HandleAccept found Error: {0}", ec.message());
		}
		StartAccept();
	}

	/* TestServerLayer */

	TestServerLayer::TestServerLayer() :
		Layer("TestServer"), m_server(nullptr), m_connection(nullptr)
	{
	}

	TestServerLayer::~TestServerLayer()
	{
		if (m_server)
			delete m_server;
	}

	//Create a server, get the connection, poll actions
	void TestServerLayer::OnAttach()
	{
		try
		{
			m_server = new TCPServer(m_context);
			m_connection = m_server->StartAccept();
			m_context.poll();
		}
		catch (std::exception& e)
		{
			NAV_INFO("ServerLayer Error: {0}", e.what());
		}
	}

	void TestServerLayer::OnDetach()
	{
	}

	//Tell to write, then poll actions
	void TestServerLayer::OnUpdate()
	{
		m_connection->Start();
		m_context.poll();
	}

}