#ifndef NAVDATA_H
#define NAVDATA_H

namespace Navigator {

	struct NavData
	{
		uint32_t longEnergy;
		uint32_t shortEnergy;
		uint64_t timestamp;
		uint32_t id;
	};

	using NavEvent = std::vector<NavData>;

}

#endif