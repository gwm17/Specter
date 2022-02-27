#include "SpectrumManager.h"

#include "implot.h"

namespace Navigator {

	SpectrumManager* SpectrumManager::s_instance = new SpectrumManager();

	SpectrumManager::SpectrumManager()
	{
	}

	SpectrumManager::~SpectrumManager()
	{
	}

	void SpectrumManager::AddHistogram(const HistogramParameters& params)
	{
		std::lock_guard<std::mutex> guard(m_managerMutex);
		if (params.y_par == "None")
			m_histoMap[params.name].reset(new Histogram1D(params));
		else
			m_histoMap[params.name].reset(new Histogram2D(params));
	}

	void SpectrumManager::RemoveHistogram(const std::string& name)
	{
		std::lock_guard<std::mutex> guard(m_managerMutex);
		m_histoMap.erase(name);
	}

	void SpectrumManager::AddCutToHistogramDraw(const std::string& cutname, const std::string& histoname)
	{
		std::lock_guard<std::mutex> guard(m_managerMutex);
		auto iter = m_histoMap.find(histoname);
		if (iter != m_histoMap.end())
			iter->second->AddCutToBeDrawn(cutname);
	}

	void SpectrumManager::AddCutToHistogramApplied(const std::string& cutname, const std::string& histoname)
	{
		std::lock_guard<std::mutex> guard(m_managerMutex);
		auto iter = m_histoMap.find(histoname);
		if (iter != m_histoMap.end())
			iter->second->AddCutToBeApplied(cutname);
	}

	void SpectrumManager::UpdateHistograms()
	{
		std::lock_guard<std::mutex> guard(m_managerMutex);
		bool cutFlag;
		for (auto& pair : m_histoMap)
		{
			cutFlag = true;
			for (auto& cutname : pair.second->GetParameters().cutsAppliedTo)
			{
				if (!IsInsideCut(cutname))
				{
					cutFlag = false;
					break;
				}
			}
			if (!cutFlag)
				continue;

			if (pair.second->Is1D())
			{
				auto iterX = m_paramMap.find(pair.second->GetXParam());
				if (iterX != m_paramMap.end() && iterX->second->validFlag)
					pair.second->FillData(iterX->second->value);
			}
			else if (pair.second->Is2D())
			{
				auto iterX = m_paramMap.find(pair.second->GetXParam());
				auto iterY = m_paramMap.find(pair.second->GetYParam());
				if (iterX != m_paramMap.end() && iterY != m_paramMap.end() &&  iterX->second->validFlag && iterY->second->validFlag)
					pair.second->FillData(iterX->second->value, iterY->second->value);
			}
		}
	}

	void SpectrumManager::ClearHistograms()
	{
		std::lock_guard<std::mutex> guard(m_managerMutex);
		for (auto& pair : m_histoMap)
			pair.second->ClearData();
	}

	void SpectrumManager::ClearHistogram(const std::string& name)
	{
		std::lock_guard<std::mutex> guard(m_managerMutex);
		auto iter = m_histoMap.find(name);
		if (iter != m_histoMap.end())
			iter->second->ClearData();
	}

	void  SpectrumManager::DrawHistogram(const std::string& name)
	{
		std::lock_guard<std::mutex> guard(m_managerMutex);
		auto iter = m_histoMap.find(name);
		if (iter != m_histoMap.end())
		{
			iter->second->Draw();
			for (auto& cutname : iter->second->GetParameters().cutsDrawnUpon)
				DrawCut(cutname);
		}
	}

	const HistogramParameters& SpectrumManager::GetHistogramParams(const std::string& name)
	{
		std::lock_guard<std::mutex> guard(m_managerMutex);
		auto iter = m_histoMap.find(name);
		if (iter != m_histoMap.end())
			return iter->second->GetParameters();
		else
			return m_nullHistoResult;
	}

	float* SpectrumManager::GetColorScaleRange(const std::string& name)
	{
		std::lock_guard<std::mutex> guard(m_managerMutex);
		auto iter = m_histoMap.find(name);
		if (iter != m_histoMap.end())
		{
			return iter->second->GetColorScaleRange();
		}

		return nullptr;
	}

    std::vector<double> SpectrumManager::GetBinData(const std::string& name)
    {
        std::lock_guard<std::mutex> guard(m_managerMutex);
        auto iter = m_histoMap.find(name);
        if (iter != m_histoMap.end())
        {
            return iter->second->GetBinData();
        }

        return std::vector<double>();
    }

	StatResults SpectrumManager::AnalyzeHistogramRegion(const std::string& name, const ImPlotRect& region)
	{
		std::lock_guard<std::mutex> guard(m_managerMutex);
		auto iter = m_histoMap.find(name);
		if (iter != m_histoMap.end())
			return iter->second->AnalyzeRegion(region.X.Min, region.X.Max, region.Y.Min, region.Y.Max);
		else
			return StatResults();
	}

	std::vector<HistogramParameters> SpectrumManager::GetListOfHistograms()
	{
		std::lock_guard<std::mutex> guard(m_managerMutex);
		std::vector<HistogramParameters> list;
		list.reserve(m_histoMap.size());
		for (auto& gram : m_histoMap)
		{
			list.push_back(gram.second->GetParameters());
		}
		return list;
	}

	void SpectrumManager::BindParameter(NavParameter& param)
	{
		std::lock_guard<std::mutex> guard(m_managerMutex);
		auto iter = m_paramMap.find(param.GetName());
		if (iter == m_paramMap.end())
		{
			m_paramMap[param.GetName()].reset(new ParameterData());
		}

		param.m_pdata = m_paramMap[param.GetName()];
	}

	void SpectrumManager::InvalidateParameters()
	{
		std::lock_guard<std::mutex> guard(m_managerMutex);
		for (auto& param : m_paramMap)
		{
			param.second->validFlag = false;
			param.second->value = 0.0;
		}
	}

	std::vector<std::string> SpectrumManager::GetListOfParameters()
	{
		std::lock_guard<std::mutex> guard(m_managerMutex);
		std::vector<std::string> list;
		list.reserve(m_paramMap.size());
		for (auto iter : m_paramMap)
		{
			list.push_back(iter.first);
		}

		return list;
	}

	void SpectrumManager::RemoveCut(const std::string& name)
	{
		std::lock_guard<std::mutex> guard(m_managerMutex);
		m_cutMap.erase(name);
		RemoveCutFromHistograms(name);
	}

	std::vector<double> SpectrumManager::GetCutXPoints(const std::string& name)
	{
		std::lock_guard<std::mutex> guard(m_managerMutex);
		std::vector<double> null_result;
		auto iter = m_cutMap.find(name);
		if (iter != m_cutMap.end())
			return iter->second->GetXValues();
		return null_result;
	}

	std::vector<double> SpectrumManager::GetCutYPoints(const std::string& name)
	{
		std::lock_guard<std::mutex> guard(m_managerMutex);
		std::vector<double> null_result;
		auto iter = m_cutMap.find(name);
		if (iter != m_cutMap.end())
			return iter->second->GetYValues();
		return null_result;
	}

	std::vector<CutParams> SpectrumManager::GetListOfCuts()
	{
		std::lock_guard<std::mutex> guard(m_managerMutex);
		std::vector<CutParams> list;
		list.reserve(m_cutMap.size());
		for (auto& entry : m_cutMap)
			list.push_back(entry.second->GetCutParams());
		return list;
	}

	/*
		Private Functions
		Can only be called from within the SpectrumManager, therefore the lock should already have been aquired by 
		whatever parent function calls them
	*/

	//private helper function; does not need thread-locked
	void SpectrumManager::RemoveCutFromHistograms(const std::string& cutname)
	{
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

	void SpectrumManager::DrawCut(const std::string& name)
	{
		auto iter = m_cutMap.find(name);
		if (iter != m_cutMap.end())
			iter->second->Draw();
	}

	bool SpectrumManager::IsInsideCut(const std::string& name)
	{
		bool result = false;
		auto iter = m_cutMap.find(name);
		if (iter != m_cutMap.end())
		{
			const std::string& xpar = iter->second->GetXParameter();
			const std::string& ypar = iter->second->GetYParameter();
			if (iter->second->Is1D())
			{
				auto iterX = m_paramMap.find(xpar);
				if (iterX != m_paramMap.end() && iterX->second->validFlag)
					result = iter->second->IsInside(iterX->second->value);
			}
			else if (iter->second->Is2D())
			{
				auto iterX = m_paramMap.find(xpar);
				auto iterY = m_paramMap.find(ypar);
				if (iterX != m_paramMap.end() && iterX->second->validFlag && iterY != m_paramMap.end() && iterY->second->validFlag)
					result = iter->second->IsInside(iterX->second->value, iterY->second->value);
			}
		}
		return result;
	}
}
