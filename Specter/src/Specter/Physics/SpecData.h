/*
	SpecData.h
	Simple data structures for the event building process in Specter. Way to create uniform data from different sources. Note that the current paradigm
	is very heavily tied to digital data aquisition systems. Any attempt to use Specter for analog systems would require a deep overhaul of the event processing.
	Most likely link to something like nscldaq.

	GWM -- Feb 2022

	Update to reflect new CAEN binary data format with headers to indicate data contents.

	GWM -- May 2022
*/
#ifndef SPECDATA_H
#define SPECDATA_H

namespace Specter {

	struct SpecData
	{
		uint32_t longEnergy = 0;
		uint32_t shortEnergy = 0;
		uint64_t calEnergy = 0;
		uint64_t timestamp = 0;
		uint32_t id = 0;
	};

	using SpecEvent = std::vector<SpecData>;

}

#endif