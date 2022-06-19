#include "CompassHit.h"

namespace Specter {

	bool Compass_IsEnergy(uint16_t header)
	{
		return (header & CompassHeaders::Energy) != 0;
	}

	bool Compass_IsEnergyShort(uint16_t header)
	{
		return (header & CompassHeaders::EnergyShort) != 0;
	}

	bool Compass_IsEnergyCalibrated(uint16_t header)
	{
		return (header & CompassHeaders::EnergyCalibrated) != 0;
	}

	bool Compass_IsWaves(uint16_t header)
	{
		return (header & CompassHeaders::EnergyCalibrated) != 0;
	}
}