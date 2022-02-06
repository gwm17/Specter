#ifndef HISTOGRAM_MAP_H
#define HISTOGRAM_MAP_H

#include "NavCore.h"
#include "Histogram.h"

#include <thread>

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
		void ClearHistograms();
		void ClearHistogram(const std::string& name);

		void DrawHistograms();
		void DrawHistogram(const std::string& name);

		const HistogramParameters& GetHistogramParams(const std::string& name);

		std::vector<HistogramParameters> GetListOfHistograms();

		static HistogramMap& GetInstance() { return *s_instance; }

	private:
		std::mutex m_histoMutex;
		std::unordered_map<std::string, std::shared_ptr<Histogram>> m_map;

		HistogramParameters m_nullResult;

		static HistogramMap* s_instance;
	};

}

#endif
