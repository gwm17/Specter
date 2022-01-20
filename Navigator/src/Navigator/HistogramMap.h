#ifndef HISTOGRAM_MAP_H
#define HISTOGRAM_MAP_H

#include "NavCore.h"
#include "Histogram.h"

namespace Navigator {

	class NAV_API HistogramMap
	{
	public:
		using Iter = std::unordered_map<std::string, std::shared_ptr<Histogram>>::iterator;

		HistogramMap();
		~HistogramMap();

		void AddHistogram(const HistogramParameters& params);
		
        void AddCutToHistogramDraw(const std::string& cutname, const std::string& histoname);
        void AddCutToHistogramApplied(const std::string& cutname, const std::string& histoname);
		void UpdateHistograms();

		void DrawHistograms();
		void DrawHistogram(const std::string& name);

		std::vector<HistogramParameters> GetListOfHistogramParams(); //thread safe access for GUI to the underlying parameters. Only needs to be called when a gram is added/removed

		static HistogramMap& GetInstance() { return *s_instance; }

		inline Iter begin() { return m_map.begin(); }
		inline Iter end() { return m_map.end(); }
		inline size_t size() { return m_map.size(); }


	private:
		std::unordered_map<std::string, std::shared_ptr<Histogram>> m_map;

		static HistogramMap* s_instance;
	};

}

#endif
