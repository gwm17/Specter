/*
	CompassHit.h
	Simple struct representing data from the CAEN CoMPASS DAQ. Note here I do not have any of the non-standard data available (calibrated energy, waveform data, etc.)

	GWM -- Feb 2022
*/
#ifndef COMPASS_HIT_H
#define COMPASS_HIT_H

#include "Navigator/Core/NavCore.h"

namespace Navigator {

	struct NAV_API CompassHit 
	{
		uint16_t board = 0;
		uint16_t channel = 0;
		uint64_t timestamp = 0;
		uint16_t lgate = 0;
		uint16_t sgate = 0;
		uint32_t flags = 0;
		uint32_t Ns = 0;
	};

}

#endif