/*
	SpectrumManager.h
	SpectrumManager is the global resource management class. Controls everything related to spectra (histograms, cuts, parameters). Since 
	the manager must traverse threads, explicit synchronoization is handled through a mutex. To this end, excessive calls to the manager should be
	avoided if possible, as this will increase the lock deadtime in the application, which is especially bad for online data sources.

	Note that SpectrumManager is a singleton. There should only ever be one SpectrumManager with a given application.

	GWM -- Feb 2022
*/
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

		//To clear all managed spectra. Note that Parameters are left untouched.
		inline void RemoveAllSpectra()
		{
			std::scoped_lock<std::mutex> guard(m_managerMutex);
			m_histoMap.clear();
			m_cutMap.clear();
		}

		/*Histogram Functions*/
		void AddHistogram(const HistogramArgs& params);
		void AddHistogramSummary(const HistogramArgs& params, const std::vector<std::string>& subhistos);
		void RemoveHistogram(const std::string& name);
		void AddCutToHistogramDraw(const std::string& cutname, const std::string& histoname);
		void AddCutToHistogramApplied(const std::string& cutname, const std::string& histoname);
		void UpdateHistograms();
		void ClearHistograms();
		void ClearHistogram(const std::string& name);
		void DrawHistogram(const std::string& name);
		const HistogramArgs& GetHistogramParams(const std::string& name);
		float* GetColorScaleRange(const std::string& name);
        std::vector<double> GetBinData(const std::string& name);
		std::vector<std::string> GetSubHistograms(const std::string& name);
		StatResults AnalyzeHistogramRegion(const std::string& name, const ImPlotRect& region);
		std::vector<HistogramArgs> GetListOfHistograms();
		/********************/

		/*Parameter Functions*/
		void BindParameter(NavParameter& param);
		void BindParameter(NavParameter& param, int nbins, double maxVal, double minVal);
		void InvalidateParameters();
		std::vector<std::string> GetListOfParameters();
		/*********************/

		/*Variable Functions*/
		void BindVariable(NavVariable& var);
		std::vector<std::string> GetListOfVariables();
		/********************/

		/*Cut Functions*/
		inline void AddCut(const CutArgs& params, double min, double max)
		{
			std::scoped_lock<std::mutex> guard(m_managerMutex);
			m_cutMap[params.name].reset(new Cut1D(params, min, max));
		}
		inline void AddCut(const CutArgs& params, const std::vector<double>& xpoints, const std::vector<double>& ypoints)
		{
			std::scoped_lock<std::mutex> guard(m_managerMutex);
			m_cutMap[params.name].reset(new Cut2D(params, xpoints, ypoints));
		}
		inline void AddCut(const CutArgs& params, const std::vector<std::string>& subhistos, double min, double max)
		{
			std::scoped_lock<std::mutex> guard(m_managerMutex);
			m_cutMap[params.name].reset(new CutSummary(params, subhistos, min, max));
		}
		void RemoveCut(const std::string& name);
		std::vector<double> GetCutXPoints(const std::string& name);
		std::vector<double> GetCutYPoints(const std::string& name);
		std::vector<std::string> GetCutSubHistograms(const std::string& cutname);
		std::vector<CutArgs> GetListOfCuts();
		/**************/

	private:
		//Only used from within manager
		void RemoveCutFromHistograms(const std::string& cutname);
		void DrawCut(const std::string& name);
		void CheckCuts();
		bool IsCutValid(const std::string& name);
		void ResetCutValidities();

		static SpectrumManager* s_instance;

		//Actual data
		std::unordered_map<std::string, std::shared_ptr<Histogram>> m_histoMap;
		std::unordered_map<std::string, std::shared_ptr<Cut>> m_cutMap;
		std::unordered_map<std::string, std::shared_ptr<ParameterData>> m_paramMap;
		std::unordered_map<std::string, std::shared_ptr<std::atomic<double>>> m_varMap;

		HistogramArgs m_nullHistoResult; //For handling bad query

		std::mutex m_managerMutex; //synchronization
	};

}

#endif
