/*
	CompassHit.h
	Simple struct representing data from the CAEN CoMPASS DAQ. Note here I do not have any of the non-standard data available (calibrated energy, waveform data, etc.)

	GWM -- Feb 2022

	Update to reflect new CAEN binary data format with headers to indicate data contents.

	GWM -- May 2022
*/
#ifndef COMPASS_HIT_H
#define COMPASS_HIT_H

#include "Specter/Core/SpecCore.h"

namespace Specter {

	struct CompassHit 
	{
		uint16_t board = 0;
		uint16_t channel = 0;
		uint64_t timestamp = 0;
		uint16_t energy = 0;
		uint16_t energyShort = 0;
		uint64_t energyCalibrated = 0;
		uint32_t flags = 0;
		uint8_t waveCode = 0;
		uint32_t Ns = 0;
		std::vector<uint16_t> samples;
	};

	//New to CoMPASS Data Format: Headers indicating what data is present.
	enum CompassHeaders
	{
		Energy = 0x0001,
		EnergyShort = 0x0002,
		EnergyCalibrated = 0x0004,
		Waves = 0x0008
	};

	bool Compass_IsEnergy(uint16_t header);
	bool Compass_IsEnergyShort(uint16_t header);
	bool Compass_IsEnergyCalibrated(uint16_t header);
	bool Compass_IsWaves(uint16_t header);
}

#endif