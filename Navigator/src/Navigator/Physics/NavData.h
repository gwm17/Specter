/*
	NavData.h
	Simple data structures for the event building process in Navigator. Way to create uniform data from different sources. Note that the current paradigm
	is very heavily tied to digital data aquisition systems. Any attempt to use Navigator for analog systems would require a deep overhaul of the event processing.
	Most likely link to something like nscldaq.

	GWM -- Feb 2022

	Update to reflect new CAEN binary data format with headers to indicate data contents.

	GWM -- May 2022
*/
#ifndef NAVDATA_H
#define NAVDATA_H

namespace Navigator {

	struct NavData
	{
		uint32_t longEnergy;
		uint32_t shortEnergy;
		uint64_t calEnergy;
		uint64_t timestamp;
		uint32_t id;
	};

	using NavEvent = std::vector<NavData>;

}

#endif