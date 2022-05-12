/*
	CompassOnlineSource.h
	A data source for online CAEN CoMPASS Data. Uses TCPClient to connect to the CoMPASS server. Data is then converted
	from the CAEN CoMPASS format to the native NavData format. Note that here we use syncrhonous since we
	need to know if the buffer is/was filled, however we use non-blocking since we don't want the entire process to hang on attempting a connection or waiting
	for data to come over the pipe. We handle the case of an un-filled buffer internally.

	IMPORTANT
	Navigator wants a unqiue ID on each hit. To do this we use the idiom:
	id = board_number * nchannels_per_board + channel_number
	This requires two things: that the class variable m_nchannels_per_board be set to match your physical digitizers, and that ALL of your
	digitizers have the SAME number of channels. By default CompassRun assumes 16 channels per board, as this is what is used with the SE-SPS setup at FoxLab.
	If you use a different set of boards, CHANGE THIS VALUE! If you use mixed boards, you will need to invent a new id scheme altogether.

	ADDITIONALLY
	CoMPASS servers provide no stream side information on the state of a transfer (verified via communication w/ CAEN). That is: there are no headers or enders on the data transfers.
	This forces us to use the size of a single CoMPASS datum (CompassHit) to determine the state of a transfer. If the read buffer size is not a whole multiple of CompassHits, the data
	was determined to be fragmented. This has a huge drawback: in general there is no guarantee that the first byte of the transferred data is the first byte of a CompassHit. This means that
	Navigator MUST be connected to the CoMPASS server BEFORE starting the aquisition. Otherwise the program could endup in a state of scrambled unpacking (also verified w/ CAEN).
	Maybe we can get them to change this? Headers reeaaally should exist for transfers like this.

	GWM -- April 2022
*/
#ifndef COMPASS_ONLINE_SOURCE_H
#define COMPASS_ONLINE_SOURCE_H

#include "Navigator/Physics/DataSource.h"
#include "Navigator/Utils/TCPClient.h"
#include "CompassHit.h"

namespace Navigator {

	class CompassOnlineSource : public DataSource
	{
	public:
		CompassOnlineSource(const std::string& hostname, const std::string& port);
		virtual ~CompassOnlineSource() override;

		virtual const NavData& GetData() override;

	private:
		void InitConnection(const std::string& hostname, const std::string& port);
		void FillBuffer();
		void GetHit();

		std::vector<char> m_currentBuffer;
		static constexpr int m_datasize = 24; //size of CoMPASS hit in bytes, change as needed (if for example you have calibrated energies)
		const int m_nchannels_per_board = 16; //IMPORTANT: Used for ID'ing channels uniquely. If you use boards with 32 or 8 or 64 channels you must change this! If you mix boards with
											  //different numbers of channels, you will have to find a different id solution.
		char* m_bufferIter;
		char* m_bufferEnd;
		CompassHit m_currentHit;

		TCPClient m_connection;

	};

}

#endif
