#ifndef SPECTRUM_SERIALIZER_H
#define SPECTRUM_SERIALIZER_H

#include "Histogram.h"
#include "Cut.h"

namespace Navigator {

	class NAV_API SpectrumSerializer
	{
	public:
		SpectrumSerializer(const std::string& filepath);
		~SpectrumSerializer();

		void SerializeData(const std::vector<HistogramParameters>& histoList, const std::vector<CutParams>& cutList);
		void DeserializeData();

		inline const std::string& GetFilename() { return m_filename; }

	private:
		std::string m_filename;
	};

}

#endif