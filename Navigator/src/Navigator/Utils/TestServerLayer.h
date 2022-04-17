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
#ifndef TEST_SERVER_LAYER_H
#define TEST_SERVER_LAYER_H

#include "Navigator/Core/Layer.h"
#include "Navigator/Physics/Caen/CompassHit.h"
#include "asio.hpp"

namespace Navigator {

	/*Server-side TCP Connection to the open port*/
	class TCPConnection
	{
	public:
		TCPConnection(asio::io_context& context);
		inline static std::shared_ptr<TCPConnection> CreateConnection(asio::io_context& context) { return std::make_shared<TCPConnection>(context); }
		inline asio::ip::tcp::socket& Socket() { return m_socket; }
		
		void Start();

	private:
		void HandleWrite(const asio::error_code& ec, size_t bytes);
		void CreateBinaryBuffer();
		void CreateBinaryBufferFragmented();

		asio::ip::tcp::socket m_socket;
		std::vector<char> m_buffer;
		CompassHit m_hit;
		static constexpr uint64_t s_timestep = 2000000;
	};

	/*Server itself*/
	class TCPServer
	{
	public:
		TCPServer(asio::io_context& context);
		inline bool IsAccepted() { return m_isAccepted;  }
		std::shared_ptr<TCPConnection> StartAccept();

	private:
		void HandleAccept(std::shared_ptr<TCPConnection> connection, const asio::error_code& error);

		asio::io_context& m_contextRef;
		asio::ip::tcp::acceptor m_acceptor;
		bool m_isAccepted;
	};

	class TestServerLayer : public Layer
	{
	public:
		TestServerLayer();
		virtual ~TestServerLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;

	private:
		asio::io_context m_context;
		TCPServer* m_server;
		std::shared_ptr<TCPConnection> m_connection;
	};
}

#endif
