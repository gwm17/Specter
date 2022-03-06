/*
	SpectrumPanel.h
	This is the big boi. Renders a panel holding all of the drawn plots. Good news is that in general only a few things really require
	any modification if new types of plots are to be rendered, basically just the zoomed in spectrum rendering.

	GWM -- Feb 2022
*/
#ifndef SPECTRUM_PANEL_H
#define SPECTRUM_PANEL_H

#include "Navigator/Histogram.h"
#include "Navigator/Cut.h"
#include "imgui.h"
#include "implot.h"

namespace Navigator {

	//Simple struct for holding a region of interest
	struct NAV_API IntegrationRegion
	{
		IntegrationRegion(const ImPlotRect& rect, const std::string& n, const std::string& hist_n) :
			region(rect), name(n), histogram_name(hist_n)
		{
		}
		ImPlotRect region;
		std::string name = "";
		std::string histogram_name = "";
	};

	class NAV_API SpectrumPanel
	{
	public:
		SpectrumPanel();
		~SpectrumPanel();
	
		bool OnImGuiRender(const std::vector<HistogramParameters>& histoList, const std::vector<CutParams>& cutList, const std::vector<std::string>& paramList);
		inline const std::string& GetZoomedOnHistogram() { return m_zoomedGram.name;  }
		inline const bool IsZoomed() { return m_zoomedFlag;  }

	private:
		void RenderCutButton();
		void RenderRemoveRegionButton();
		void RemoveSelectedRegion(const std::string& region);
		std::vector<HistogramParameters> m_selectedGrams;
		std::vector<IntegrationRegion> m_integralRegions;
		bool m_zoomedFlag;
        bool m_cutModeFlag;
		HistogramParameters m_zoomedGram;
		int m_tableSizes[2];
		int m_totalSlots;
		int m_nRegions;
		CutParams m_newCutParams;
		std::vector<double> m_newCutX;
		std::vector<double> m_newCutY;
	};

}

#endif
