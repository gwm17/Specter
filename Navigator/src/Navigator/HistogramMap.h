#ifndef HISTOGRAM_MAP_H
#define HISTOGRAM_MAP_H

#include "NavCore.h"
#include "Histogram.h"
#include <thread>

namespace Navigator {

	class NAV_API HistogramMap
	{
	public:

		using Iter = std::unordered_map<std::string, std::unique_ptr<Histogram>>::iterator;

		HistogramMap();
		~HistogramMap();

		inline void AddHistogram(const std::string& name, const std::string& param, int bins, double min, double max) 
		{ 
			std::lock_guard<std::mutex> guard(m_histMutex);
			m_map[name].reset(new Histogram1D(name, param, bins, min, max)); 
		}
		inline void AddHistogram(const std::string& name, const std::string& paramx, const std::string& paramy, int bins_x, double min_x, double max_x,
								 int bins_y, double min_y, double max_y) 
		{
			std::lock_guard<std::mutex> guard(m_histMutex);
			m_map[name].reset(new Histogram2D(name, paramx, paramy, bins_x, min_x, max_x, bins_y, min_y, max_y));
		}

        void AddCutToHistogramDraw(const std::string& cutname, const std::string& histoname);
        void AddCutToHistogramApplied(const std::string& cutname, const std::string& histoname);
		void UpdateHistograms();

		void DrawHistograms();
		void DrawHistogram(const std::string& name);

		std::vector<HistogramParameters> GetListOfHistogramParams(); //thread safe access for GUI to the underlying parameters. Only needs to be called when a gram is added/removed

		/*Not in general thread safe*/
		inline Iter begin() { return m_map.begin(); } 
		inline Iter end() { return m_map.end();  }

	private:
		std::unordered_map<std::string, std::unique_ptr<Histogram>> m_map;
		std::mutex m_histMutex;
	};

}

#endif
