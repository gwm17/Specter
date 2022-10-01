/*
	CompassOnlineSource.h
	A data source for online CAEN CoMPASS Data. Uses TCPClient to connect to the CoMPASS server. Data is then converted
	from the CAEN CoMPASS format to the native NavData format. Note that here we use syncrhonous since we
	need to know if the buffer is/was filled, however we use non-blocking since we don't want the entire process to hang on attempting a connection or waiting
	for data to come over the pipe. We handle the case of an un-filled buffer internally.

	ADDITIONALLY
	CoMPASS servers provide no stream side information on the state of a transfer (verified via communication w/ CAEN). That is: there are no headers or enders on the data transfers.
	This forces us to use the size of a single CoMPASS datum (CompassHit) to determine the state of a transfer. If the read buffer size is not a whole multiple of CompassHits, the data
	was determined to be fragmented. This has a huge drawback: in general there is no guarantee that the first byte of the transferred data is the first byte of a CompassHit. This means that
	Specter MUST be connected to the CoMPASS server BEFORE starting the aquisition. Otherwise the program could endup in a state of scrambled unpacking (also verified w/ CAEN).
	Maybe we can get them to change this? Headers reeaaally should exist for transfers like this.

	GWM -- April 2022

	Update to reflect new CAEN binary data format with headers to indicate data contents. Note that as prev. mentioned, no headers, so cannot rely on data stream to indicate state. State must be selected 
	by user at UI when creating source. Cannot support waves atm. No way to predict size of first event to calibrate the number of samples for the stream (or guarantee that they will be constant for duration
	of Specter's runtime). Best to use the CoMPASSPlot for waves.

	GWM -- May 2022

	Make it so that number of channels per board is no longer fixed. Use pairing function defined in Utils/Functions.h to generate a UUID for each board channel/pair.

	GWM -- Oct 2022
*/
#ifndef COMPASS_ONLINE_SOURCE_H
#define COMPASS_ONLINE_SOURCE_H

#include "Specter/Physics/DataSource.h"
#include "Specter/Utils/TCPClient.h"
#include "CompassHit.h"

namespace Specter {

	class CompassOnlineSource : public DataSource
	{
	public:
		CompassOnlineSource(const std::string& hostname, const std::string& port, uint16_t header);
		virtual ~CompassOnlineSource() override;

		virtual const SpecData& GetData() override;

	private:
		void InitConnection(const std::string& hostname, const std::string& port);
		void FillBuffer();
		void GetHit();
		void ReadHeader();

		std::vector<char> m_currentBuffer;
		uint16_t m_header;
		int m_datasize; //size of CoMPASS hit in bytes, set by header arg
		char* m_bufferIter;
		char* m_bufferEnd;
		CompassHit m_currentHit;

		TCPClient m_connection;

	};

}

#endif
