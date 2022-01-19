#ifndef SPECTRUM_PANEL_H
#define SPECTRUM_PANEL_H

#include "Navigator/HistogramMap.h"
#include "imgui.h"

namespace Navigator {

	class NAV_API SpectrumPanel
	{
	public:
		SpectrumPanel();
		~SpectrumPanel();
	
		void OnImGuiRender();
		inline const std::string& GetZoomedOnHistogram() { return m_zoomedGram;  }
		inline const bool IsZoomed() { return m_zoomedFlag;  }
		inline void UpdateActiveList(const std::vector<HistogramParameters>& list) { m_activeList = list;  }

	private:
		std::vector<HistogramParameters> m_activeList; //This is where we get our info from. Reduces thread crossings
		std::vector<std::string> m_selectedGrams;
		bool m_zoomedFlag;
		std::string m_zoomedGram;
		int m_tableSizes[2];
		int m_totalSlots;
	};

}

#endif