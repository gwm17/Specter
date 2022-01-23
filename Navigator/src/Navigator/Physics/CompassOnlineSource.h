#ifndef COMPASS_ONLINE_SOURCE_H
#define COMPASS_ONLINE_SOURCE_H

#include "DataSource.h"
#include "asio/include/asio.hpp"

namespace Navigator {

	class CompassOnlineSource : public DataSource
	{
	public:
		CompassOnlineSource(const std::string& hostname, const std::string& port);
		virtual ~CompassOnlineSource() override;

		virtual const CompassHit& GetData() override;

	private:
		void InitSocket(const std::string& hostname, const std::string& port);
		void FillBuffer();
		void GetHit();
		std::vector<char> m_buffer;
		static constexpr size_t m_bufferSize = 1000000;
		char* m_bufferIter;
		char* m_bufferEnd;
		CompassHit m_currentHit;

		asio::io_context m_socketContext;
		asio::ip::tcp::socket m_socket;

	};

}

#endif