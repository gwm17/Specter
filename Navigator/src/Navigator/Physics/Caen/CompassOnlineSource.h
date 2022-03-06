/*
	CompassOnlineSource.h
	A data source for online CAEN CoMPASS Data. Creates a tcp socket, connects to the remote source, and pulls data to a buffer. Data is then converted
	from the CAEN CoMPASS format to the native NavData format. Uses asio as the networking library (see asio docs).

	IMPORTANT
	Navigator wants a unqiue ID on each hit. To do this we use the idiom:
	id = board_number * nchannels_per_board + channel_number
	This requires two things: that the class variable m_nchannels_per_board be set to match your physical digitizers, and that ALL of your
	digitizers have the SAME number of channels. By default CompassRun assumes 16 channels per board, as this is what is used with the SE-SPS setup at FoxLab.
	If you use a different set of boards, CHANGE THIS VALUE! If you use mixed boards, you will need to invent a new id scheme altogether.

	GWM -- Feb 2022
*/
#ifndef COMPASS_ONLINE_SOURCE_H
#define COMPASS_ONLINE_SOURCE_H

#include "Navigator/Physics/DataSource.h"
#include "CompassHit.h"
#include "asio.hpp"

namespace Navigator {

	class CompassOnlineSource : public DataSource
	{
	public:
		CompassOnlineSource(const std::string& hostname, const std::string& port);
		virtual ~CompassOnlineSource() override;

		virtual const NavData& GetData() override;

	private:
		void InitSocket(const std::string& hostname, const std::string& port);
		void FillBuffer();
		void GetHit();
		std::vector<char> m_buffer;
		static constexpr size_t m_bufferSize = 1000000; //Max amount of data we allow the source to buffer in. I don't think this should ever be maxed?
		const int m_nchannels_per_board = 16; //IMPORTANT: Used for ID'ing channels uniquely. If you use boards with 32 or 8 or 64 channels you must change this! If you mix boards with
											  //different numbers of channels, you will have to find a different id solution.
		char* m_bufferIter;
		char* m_bufferEnd;
		CompassHit m_currentHit;

		asio::io_context m_socketContext;
		asio::ip::tcp::socket m_socket;

	};

}

#endif
