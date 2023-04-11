/*
	SpectrumSerializer.h
	SpectrumSerializer class providing method to write/read spectra (histograms and cuts) to/from a .yaml file. These are YAML files.
	Note that by virtue of the way that cuts work, they are written first.

	A couple of notes:
	- Writing/reading data is a pretty expensive concept from a thread-locking perspective. Not recommended for use during active analysis.
	- Deserializing (reading) by default removes all current histograms and cuts. This avoids any issues with collisions, but may not always be desireable.
	- There is no intrinsic checking of whether a parameter for a cut/histogram to be loaded exists within our framework. If you load something and the histograms
	  don't fill, it is most likely due to the parameter not being defined in the current project.

	GWM -- Feb 2022
*/
#ifndef SPECTRUM_SERIALIZER_H
#define SPECTRUM_SERIALIZER_H

#include "Histogram.h"
#include "Cut.h"
#include "SpectrumManager.h"

namespace Specter {

	class SpectrumSerializer
	{
	public:
		SpectrumSerializer(const std::string& filepath);
		~SpectrumSerializer();

		void SerializeData(const SpectrumManager::Ref& manager);
		void DeserializeData(SpectrumManager::Ref& manager);

		inline const std::string& GetFilename() { return m_filename; }

	private:
		std::string m_filename;
	};

}

#endif