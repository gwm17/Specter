#include "HistogramMap.h"
#include "ParameterMap.h"

#include "implot.h"

namespace Navigator {

	HistogramMap* HistogramMap::s_instance = new HistogramMap();

	HistogramMap::HistogramMap()
	{
	}

	HistogramMap::~HistogramMap()
	{
	}

	void HistogramMap::AddHistogram(const HistogramParameters& params)
	{
		std::lock_guard<std::mutex> guard(m_histoMutex);
		if (params.y_par == "None")
			m_map[params.name].reset(new Histogram1D(params));
		else
			m_map[params.name].reset(new Histogram2D(params));
	}

	void HistogramMap::RemoveHistogram(const std::string& name)
	{
		std::lock_guard<std::mutex> guard(m_histoMutex);
		m_map.erase(name);
	}

    void HistogramMap::AddCutToHistogramDraw(const std::string& cutname, const std::string& histoname)
    {
		std::lock_guard<std::mutex> guard(m_histoMutex);
        auto iter = m_map.find(histoname);
        if(iter != m_map.end())
            iter->second->AddCutToBeDrawn(cutname);
    }

    void HistogramMap::AddCutToHistogramApplied(const std::string& cutname, const std::string& histoname)
    {
		std::lock_guard<std::mutex> guard(m_histoMutex);
        auto iter = m_map.find(histoname);
        if(iter != m_map.end())
            iter->second->AddCutToBeApplied(cutname);
    }

	void HistogramMap::RemoveCutFromHistograms(const std::string& cutname)
	{
		std::lock_guard<std::mutex> guard(m_histoMutex);
		for (auto& gram : m_map)
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

	void HistogramMap::UpdateHistograms()
	{
		std::lock_guard<std::mutex> guard(m_histoMutex);
		for (auto& pair : m_map)
			pair.second->FillData();
	}

	void HistogramMap::ClearHistograms()
	{
		std::lock_guard<std::mutex> guard(m_histoMutex);
		for(auto& pair : m_map)
			pair.second->ClearData();
	}

	void HistogramMap::ClearHistogram(const std::string& name)
	{
		std::lock_guard<std::mutex> guard(m_histoMutex);
		auto iter = m_map.find(name);
		if(iter != m_map.end())
			iter->second->ClearData();
	}

	const HistogramParameters& HistogramMap::GetHistogramParams(const std::string& name)
	{
		
		std::lock_guard<std::mutex> guard(m_histoMutex);
		auto iter = m_map.find(name);
		if (iter != m_map.end())
			return iter->second->GetParameters();
		else
			return m_nullResult;
	}

	StatResults HistogramMap::AnalyzeHistogramRegion(const std::string& name, const ImPlotRect& region)
	{
		std::lock_guard<std::mutex> guard(m_histoMutex);
		auto iter = m_map.find(name);
		if (iter != m_map.end())
			return iter->second->AnalyzeRegion(region.X.Min, region.X.Max, region.Y.Min, region.Y.Max);
		else
			return StatResults();
	}

	std::vector<HistogramParameters> HistogramMap::GetListOfHistograms()
	{
		std::lock_guard<std::mutex> guard(m_histoMutex);
		std::vector<HistogramParameters> list;
		list.reserve(m_map.size());
		for(auto& gram : m_map)
		{
			list.push_back(gram.second->GetParameters());
		}
		return list;
	}

	//Only to be used within ImGui context!!
	void Navigator::HistogramMap::DrawHistograms()
	{
		std::lock_guard<std::mutex> guard(m_histoMutex);
		for (auto& pair : m_map)
			pair.second->Draw();
	}

	//Only to be used within ImGui context!!
	void Navigator::HistogramMap::DrawHistogram(const std::string& name)
	{
		std::lock_guard<std::mutex> guard(m_histoMutex);
		auto iter = m_map.find(name);
		if (iter != m_map.end())
			iter->second->Draw();
	}
}
