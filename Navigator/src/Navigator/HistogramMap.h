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

		void RemoveHistogram(const std::string& name);

		
        void AddCutToHistogramDraw(const std::string& cutname, const std::string& histoname);
        void AddCutToHistogramApplied(const std::string& cutname, const std::string& histoname);
		void RemoveCutFromHistograms(const std::string& cutname);

		void UpdateHistograms();

		void DrawHistograms();
		void DrawHistogram(const std::string& name);

		const HistogramParameters& GetHistogramParams(const std::string& name);

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
