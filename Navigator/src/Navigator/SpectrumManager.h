#ifndef SPECTRUM_MANAGER_H
#define SPECTRUM_MANAGER_H

#include "Histogram.h"
#include "Cut.h"
#include "Parameter.h"

#include <thread>

struct ImPlotRect;

namespace Navigator {

	class NAV_API SpectrumManager
	{
	public:
		SpectrumManager();
		~SpectrumManager();

		inline static SpectrumManager& GetInstance() { return *s_instance;  }

		void AddHistogram(const HistogramParameters& params);
		void RemoveHistogram(const std::string& name);
		void AddCutToHistogramDraw(const std::string& cutname, const std::string& histoname);
		void AddCutToHistogramApplied(const std::string& cutname, const std::string& histoname);
		void UpdateHistograms();
		void ClearHistograms();
		void ClearHistogram(const std::string& name);
		void DrawHistogram(const std::string& name);
		const HistogramParameters& GetHistogramParams(const std::string& name);
		StatResults AnalyzeHistogramRegion(const std::string& name, const ImPlotRect& region);
		std::vector<HistogramParameters> GetListOfHistograms();

		void BindParameter(NavParameter& param);
		void InvalidateParameters();
		std::vector<std::string> GetListOfParameters();

		inline void AddCut(const CutParams& params, double min, double max)
		{
			std::lock_guard<std::mutex> guard(m_managerMutex);
			m_cutMap[params.name].reset(new Cut1D(params, min, max));
		}
		inline void AddCut(const CutParams& params, const std::vector<double>& xpoints, const std::vector<double>& ypoints)
		{
			std::lock_guard<std::mutex> guard(m_managerMutex);
			m_cutMap[params.name].reset(new Cut2D(params, xpoints, ypoints));
		}
		void RemoveCut(const std::string& name);
		std::vector<double> GetCutXPoints(const std::string& name);
		std::vector<double> GetCutYPoints(const std::string& name);
		std::vector<CutParams> GetListOfCuts();

	private:
		void RemoveCutFromHistograms(const std::string& cutname);
		void DrawCut(const std::string& name);
		bool IsInsideCut(const std::string& name);

		static SpectrumManager* s_instance;

		std::unordered_map<std::string, std::shared_ptr<Histogram>> m_histoMap;
		std::unordered_map<std::string, std::shared_ptr<Cut>> m_cutMap;
		std::unordered_map<std::string, std::shared_ptr<ParameterData>> m_paramMap;

		HistogramParameters m_nullHistoResult;

		std::mutex m_managerMutex;
	};

}

#endif