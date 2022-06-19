/*
	SpectrumManager.cpp
	SpectrumManager is the global resource management class. Controls everything related to spectra (histograms, cuts, parameters). Since
	the manager must traverse threads, explicit synchronoization is handled through a mutex. To this end, excessive calls to the manager should be
	avoided if possible, as this will increase the lock deadtime in the application, which is especially bad for online data sources.

	Note that SpectrumManager is a singleton. There should only ever be one SpectrumManager with a given application.

	GWM -- Feb 2022
*/
#include "SpectrumManager.h"

#include "implot.h"

namespace Specter {

	SpectrumManager* SpectrumManager::s_instance = new SpectrumManager();

	SpectrumManager::SpectrumManager()
	{
	}

	SpectrumManager::~SpectrumManager()
	{
	}

	/*************Histogram Functions Begin*************/

	void SpectrumManager::AddHistogram(const HistogramArgs& params)
	{
		SPEC_PROFILE_FUNCTION();
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		if (params.y_par == "None") //Check dimensionality
			m_histoMap[params.name].reset(new Histogram1D(params));
		else
			m_histoMap[params.name].reset(new Histogram2D(params));
	}

	void SpectrumManager::AddHistogramSummary(const HistogramArgs& params, const std::vector<std::string>& subhistos)
	{
		SPEC_PROFILE_FUNCTION();
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		m_histoMap[params.name].reset(new HistogramSummary(params, subhistos));
	}

	void SpectrumManager::RemoveHistogram(const std::string& name)
	{
		SPEC_PROFILE_FUNCTION();
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		m_histoMap.erase(name);
	}

	void SpectrumManager::AddCutToHistogramDraw(const std::string& cutname, const std::string& histoname)
	{
		SPEC_PROFILE_FUNCTION();
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		auto iter = m_histoMap.find(histoname);
		if (iter != m_histoMap.end())
			iter->second->AddCutToBeDrawn(cutname);
	}

	void SpectrumManager::AddCutToHistogramApplied(const std::string& cutname, const std::string& histoname)
	{
		SPEC_PROFILE_FUNCTION();
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		auto iter = m_histoMap.find(histoname);
		if (iter != m_histoMap.end())
			iter->second->AddCutToBeApplied(cutname);
	}

	//Use this to fill histograms. Currently can only be filled in bulk; maybe a use case for individual fills?
	void SpectrumManager::UpdateHistograms()
	{
		SPEC_PROFILE_FUNCTION();
		std::scoped_lock<std::mutex> guard(m_managerMutex);

		//Set state of all cuts for the event
		CheckCuts();

		bool cutFlag;
		for (auto& pair : m_histoMap)
		{
			cutFlag = true;
			for (auto& cutname : pair.second->GetParameters().cutsAppliedTo) //check the associated cuts
			{
				if (!IsCutValid(cutname))
				{
					cutFlag = false;
					break;
				}
			}
			if (!cutFlag)
				continue;

			switch (pair.second->GetType())
			{
				case SpectrumType::Histo1D:
				{
					auto iterX = m_paramMap.find(pair.second->GetXParam());
					if (iterX != m_paramMap.end() && iterX->second->validFlag)
						pair.second->FillData(iterX->second->value);
					break;
				}
				case SpectrumType::Histo2D:
				{
					auto iterX = m_paramMap.find(pair.second->GetXParam());
					auto iterY = m_paramMap.find(pair.second->GetYParam());
					if (iterX != m_paramMap.end() && iterY != m_paramMap.end() && iterX->second->validFlag && iterY->second->validFlag)
						pair.second->FillData(iterX->second->value, iterY->second->value);
					break;
				}
				case SpectrumType::Summary:
				{
					const std::vector<std::string>& subhistos = std::static_pointer_cast<HistogramSummary>(pair.second)->GetSubHistograms();
					for (size_t i = 0; i < subhistos.size(); i++)
					{
						auto iterX = m_paramMap.find(subhistos[i]);
						if (iterX != m_paramMap.end() && iterX->second->validFlag)
							pair.second->FillData(iterX->second->value, i + 0.5); //avoid floating point conversion issues
					}
					break;
				}
				case SpectrumType::None:
				{
					SPEC_WARN("Found a spectrum with None type!");
					break;
				}
			}
		}

		//Reset the state of all cuts in preparation for next event
		ResetCutValidities();
	}

	void SpectrumManager::ClearHistograms()
	{
		SPEC_PROFILE_FUNCTION();
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		for (auto& pair : m_histoMap)
			pair.second->ClearData();
	}

	void SpectrumManager::ClearHistogram(const std::string& name)
	{
		SPEC_PROFILE_FUNCTION();
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		auto iter = m_histoMap.find(name);
		if (iter != m_histoMap.end())
			iter->second->ClearData();
	}

	void  SpectrumManager::DrawHistogram(const std::string& name)
	{
		SPEC_PROFILE_FUNCTION();
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		auto iter = m_histoMap.find(name);
		if (iter != m_histoMap.end())
		{
			iter->second->Draw();
			for (auto& cutname : iter->second->GetParameters().cutsDrawnUpon) //Draw all cuts made upon the histogram
				DrawCut(cutname);
		}
	}

	const HistogramArgs& SpectrumManager::GetHistogramParams(const std::string& name)
	{
		SPEC_PROFILE_FUNCTION();
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		auto iter = m_histoMap.find(name);
		if (iter != m_histoMap.end())
			return iter->second->GetParameters();
		else
			return m_nullHistoResult;
	}

	//For 2D spectra, we want to allow zooming along the z-axis (color)
	float* SpectrumManager::GetColorScaleRange(const std::string& name)
	{
		SPEC_PROFILE_FUNCTION();
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		auto iter = m_histoMap.find(name);
		if (iter != m_histoMap.end())
		{
			return iter->second->GetColorScaleRange();
		}

		return nullptr;
	}

    std::vector<double> SpectrumManager::GetBinData(const std::string& name)
    {
        std::scoped_lock<std::mutex> guard(m_managerMutex);
        auto iter = m_histoMap.find(name);
        if (iter != m_histoMap.end())
        {
            return iter->second->GetBinData();
        }

        return std::vector<double>();
    }

	std::vector<std::string> SpectrumManager::GetSubHistograms(const std::string& name)
	{
		SPEC_PROFILE_FUNCTION();
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		auto iter = m_histoMap.find(name);
		if (iter != m_histoMap.end() && iter->second->GetType() == SpectrumType::Summary)
		{
			auto gram = std::static_pointer_cast<HistogramSummary>(iter->second);
			return gram->GetSubHistograms();
		}
		return std::vector<std::string>();
	}

	//Pass through for stats
	StatResults SpectrumManager::AnalyzeHistogramRegion(const std::string& name, const ImPlotRect& region)
	{
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		auto iter = m_histoMap.find(name);
		if (iter != m_histoMap.end())
			return iter->second->AnalyzeRegion(region.X.Min, region.X.Max, region.Y.Min, region.Y.Max);
		else
			return StatResults();
	}

	//This function allows us to obtain the key histogram info in a list, avoiding excessive manager calls and thread-locks
	//in something like the Editor.
	std::vector<HistogramArgs> SpectrumManager::GetListOfHistograms()
	{
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		std::vector<HistogramArgs> list;
		list.reserve(m_histoMap.size());
		for (auto& gram : m_histoMap)
		{
			list.push_back(gram.second->GetParameters());
		}
		return list;
	}

	/*************Histogram Functions End*************/

	/*************Graph Functions Begin*************/

	void SpectrumManager::AddGraph(const GraphArgs& args)
	{
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		m_graphMap[args.name].reset(new ScalerGraph(args));
	}

	void SpectrumManager::RemoveGraph(const std::string& name)
	{
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		m_graphMap.erase(name);
	}

	void SpectrumManager::UpdateGraphs(const Timestep& step)
	{
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		uint64_t scalerVal;
		for (auto& graph : m_graphMap)
		{
			auto& scalerIter = m_scalerMap.find(graph.second->GetScaler());
			if (scalerIter != m_scalerMap.end())
			{
				scalerVal = *(scalerIter->second);
				graph.second->UpdatePoints(step, scalerVal);
			}
		}
	}

	void SpectrumManager::ClearGraphs()
	{
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		for (auto& graph : m_graphMap)
			graph.second->Clear();
	}

	void SpectrumManager::ClearGraph(const std::string& name)
	{
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		auto& iter = m_graphMap.find(name);
		if (iter != m_graphMap.end())
			iter->second->Clear();
	}

	void SpectrumManager::DrawGraph(const std::string& name)
	{
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		auto& iter = m_graphMap.find(name);
		if (iter != m_graphMap.end())
			iter->second->Draw();
	}

	const GraphArgs& SpectrumManager::GetGraphArgs(const std::string& name)
	{
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		auto& iter = m_graphMap.find(name);
		if (iter != m_graphMap.end())
			return iter->second->GetArgs();
		return m_nullGraphResult;
	}

	std::vector<GraphArgs> SpectrumManager::GetListOfGraphs()
	{
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		std::vector<GraphArgs> list;
		list.reserve(m_graphMap.size());
		for (auto& graph : m_graphMap)
			list.push_back(graph.second->GetArgs());
		return list;
	}

	/*************Graph Functions End*************/

	/*************Parameter Functions Begin*************/

	//Bind a Parameter instance to the manager. If the Parameter doesn't exist, make a new one, otherwise attach to extant memory
	void SpectrumManager::BindParameter(Parameter& param)
	{
		SPEC_PROFILE_FUNCTION();
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		auto iter = m_paramMap.find(param.GetName());
		if (iter == m_paramMap.end())
		{
			m_paramMap[param.GetName()].reset(new ParameterData());
		}

		param.m_pdata = m_paramMap[param.GetName()];
	}

	//Bind a Parameter instance to the manager. If the Parameter doesn't exist, make a new one, otherwise attach to extant memory
	//Additionally, make a default 1D histogram for the parameter (histogram has same name as parameter)
	void SpectrumManager::BindParameter(Parameter& param, int nbins, double minVal, double maxVal)
	{
		SPEC_PROFILE_FUNCTION();
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		auto iter = m_paramMap.find(param.GetName());
		if (iter == m_paramMap.end())
		{
			m_paramMap[param.GetName()].reset(new ParameterData());
		}

		param.m_pdata = m_paramMap[param.GetName()];

		auto histoIter = m_histoMap.find(param.GetName());
		if (histoIter == m_histoMap.end())
		{
			HistogramArgs histo(param.GetName(), param.GetName(), nbins, minVal, maxVal);
			m_histoMap[param.GetName()].reset(new Histogram1D(histo));
		}
	}

	//Once an analysis pass is done and histograms filled, reset all parameters
	void SpectrumManager::InvalidateParameters()
	{
		SPEC_PROFILE_FUNCTION();
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		for (auto& param : m_paramMap)
		{
			param.second->validFlag = false;
			param.second->value = 0.0;
		}
	}

	//Similar to GetListOfHistograms, see that documentation
	std::vector<std::string> SpectrumManager::GetListOfParameters()
	{
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		std::vector<std::string> list;
		list.reserve(m_paramMap.size());
		for (auto iter : m_paramMap)
		{
			list.push_back(iter.first);
		}

		return list;
	}

	/*************Parameter Functions End*************/

	/*************Variable Functions Begin*************/

	void SpectrumManager::BindVariable(Variable& var)
	{
		SPEC_PROFILE_FUNCTION();
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		auto iter = m_varMap.find(var.GetName());
		if (iter == m_varMap.end())
		{
			m_varMap[var.GetName()].reset(new std::atomic<double>(0.0));
		}
		var.m_pdata = m_varMap[var.GetName()];
	}

	std::vector<std::string> SpectrumManager::GetListOfVariables()
	{
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		std::vector<std::string> list;
		list.reserve(m_varMap.size());
		for (auto iter : m_varMap)
			list.push_back(iter.first);
		return list;
	}

	/*************Variable Functions End*************/

	void SpectrumManager::BindScaler(Scaler& scaler)
	{
		SPEC_PROFILE_FUNCTION();
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		auto iter = m_scalerMap.find(scaler.GetName());
		if (iter == m_scalerMap.end())
		{
			m_scalerMap[scaler.GetName()].reset(new std::atomic<uint64_t>(0));
		}
		scaler.m_pdata = m_scalerMap[scaler.GetName()];
	}

	void SpectrumManager::ResetScalers()
	{
		SPEC_PROFILE_FUNCTION();
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		for (auto& scaler : m_scalerMap)
		{
			*(scaler.second) = 0;
		}
	}

	std::vector<std::string> SpectrumManager::GetListOfScalers()
	{
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		std::vector<std::string> list;
		list.reserve(m_scalerMap.size());
		for (auto& iter : m_scalerMap)
			list.push_back(iter.first);
		return list;
	}

	/*************Cut Functions Begin*************/

	void SpectrumManager::AddCut(const CutArgs& params, double min, double max)
	{
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		m_cutMap[params.name].reset(new Cut1D(params, min, max));
	}

	void SpectrumManager::AddCut(const CutArgs& params, const std::vector<double>& xpoints, const std::vector<double>& ypoints)
	{
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		m_cutMap[params.name].reset(new Cut2D(params, xpoints, ypoints));
	}

	void SpectrumManager::AddCut(const CutArgs& params, const std::vector<std::string>& subhistos, double min, double max)
	{
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		m_cutMap[params.name].reset(new CutSummary(params, subhistos, min, max));
	}

	void SpectrumManager::RemoveCut(const std::string& name)
	{
		SPEC_PROFILE_FUNCTION();
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		m_cutMap.erase(name);
		RemoveCutFromHistograms(name); //Once a cut is gone, remove all references to it.
	}

	std::vector<double> SpectrumManager::GetCutXPoints(const std::string& name)
	{
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		std::vector<double> null_result;
		auto iter = m_cutMap.find(name);
		if (iter != m_cutMap.end())
			return iter->second->GetXValues();
		return null_result;
	}

	std::vector<double> SpectrumManager::GetCutYPoints(const std::string& name)
	{
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		std::vector<double> null_result;
		auto iter = m_cutMap.find(name);
		if (iter != m_cutMap.end())
			return iter->second->GetYValues();
		return null_result;
	}

	std::vector<std::string> SpectrumManager::GetCutSubHistograms(const std::string& cutname)
	{
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		auto iter = m_cutMap.find(cutname);
		if (iter != m_cutMap.end() && iter->second->GetType() == CutType::CutSummaryAny || iter->second->GetType() == CutType::CutSummaryAll)
		{
			auto cut = std::static_pointer_cast<CutSummary>(iter->second);
			return cut->GetSubHistograms();
		}
		return std::vector<std::string>();
	}

	//Similar to GetListOfHistograms, see that documentation
	std::vector<CutArgs> SpectrumManager::GetListOfCuts()
	{
		std::scoped_lock<std::mutex> guard(m_managerMutex);
		std::vector<CutArgs> list;
		list.reserve(m_cutMap.size());
		for (auto& entry : m_cutMap)
			list.push_back(entry.second->GetCutArgs());
		return list;
	}

	/*************Cut Functions End*************/

	/*
		Private Functions
		Can only be called from within the SpectrumManager, therefore the lock should already have been aquired by 
		whatever parent function calls them. No explicit synchronization.
	*/

	//Can only be called by RemoveCut currently. May be a use case where this should be promoted to public to on the fly mod a gram.
	void SpectrumManager::RemoveCutFromHistograms(const std::string& cutname)
	{
		SPEC_PROFILE_FUNCTION();
		for (auto& gram : m_histoMap)
		{
			auto& params = gram.second->GetParameters();
			for (size_t i = 0; i < params.cutsDrawnUpon.size(); ++i)
			{
				if (params.cutsDrawnUpon[i] == cutname)
				{
					params.cutsDrawnUpon.erase(params.cutsDrawnUpon.begin() + i);
				}
			}
			for (size_t i = 0; i < params.cutsAppliedTo.size(); ++i)
			{
				if (params.cutsAppliedTo[i] == cutname)
				{
					params.cutsAppliedTo.erase(params.cutsAppliedTo.begin() + i);
				}
			}
		}
	}

	//Obv. only need to draw a cut if its parent histogram is drawn.
	void SpectrumManager::DrawCut(const std::string& name)
	{
		SPEC_PROFILE_FUNCTION();
		auto iter = m_cutMap.find(name);
		if (iter != m_cutMap.end())
			iter->second->Draw();
	}

	//Set the state of the cuts for the current event. Called by the 
	void SpectrumManager::CheckCuts()
	{
		SPEC_PROFILE_FUNCTION();
		for (auto& iter : m_cutMap)
		{
			const std::string& xpar = iter.second->GetXParameter();
			const std::string& ypar = iter.second->GetYParameter();
			switch (iter.second->GetType())
			{
				case CutType::Cut1D:
				{
					auto iterX = m_paramMap.find(xpar);
					if (iterX != m_paramMap.end() && iterX->second->validFlag)
						iter.second->IsInside(iterX->second->value);
					break;
				}
				case CutType::Cut2D:
				{
					auto iterX = m_paramMap.find(xpar);
					auto iterY = m_paramMap.find(ypar);
					if (iterX != m_paramMap.end() && iterX->second->validFlag && iterY != m_paramMap.end() && iterY->second->validFlag)
						iter.second->IsInside(iterX->second->value, iterY->second->value);
					break;
				}
				case CutType::CutSummaryAll:
				{
					std::vector<std::string> paramlist = std::static_pointer_cast<CutSummary>(iter.second)->GetSubHistograms();
					for (auto& param : paramlist)
					{
						auto iterX = m_paramMap.find(param);
						if (iterX != m_paramMap.end() && iterX->second->validFlag)
						{
							iter.second->IsInside(iterX->second->value);
							if (!iter.second->IsValid())
								break;
						}
					}
					break;
				}
				case CutType::CutSummaryAny:
				{
					std::vector<std::string> paramlist = std::static_pointer_cast<CutSummary>(iter.second)->GetSubHistograms();
					for (auto& param : paramlist)
					{
						auto iterX = m_paramMap.find(param);
						if (iterX != m_paramMap.end() && iterX->second->validFlag)
						{
							iter.second->IsInside(iterX->second->value);
							if (iter.second->IsValid())
								break;
						}
					}
					break;
				}
				case CutType::None:
				{
					SPEC_WARN("Found a cut with None type!");
					break;
				}
			}
		}
	}

	bool SpectrumManager::IsCutValid(const std::string& name)
	{
		SPEC_PROFILE_FUNCTION();
		auto iter = m_cutMap.find(name);
		if (iter != m_cutMap.end())
			return iter->second->IsValid();
		return false;
	}

	void SpectrumManager::ResetCutValidities()
	{
		for (auto& iter : m_cutMap)
		{
			iter.second->ResetValidity();
		}
	}
}
