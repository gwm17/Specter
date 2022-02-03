#include "HistogramMap.h"
#include "ParameterMap.h"

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
		if (params.y_par == "None")
			m_map[params.name].reset(new Histogram1D(params));
		else
			m_map[params.name].reset(new Histogram2D(params));
	}

	void HistogramMap::RemoveHistogram(const std::string& name)
	{
		m_map.erase(name);
	}

    void HistogramMap::AddCutToHistogramDraw(const std::string& cutname, const std::string& histoname)
    {
        auto iter = m_map.find(histoname);
        if(iter != m_map.end())
            iter->second->AddCutToBeDrawn(cutname);
    }

    void HistogramMap::AddCutToHistogramApplied(const std::string& cutname, const std::string& histoname)
    {
        auto iter = m_map.find(histoname);
        if(iter != m_map.end())
            iter->second->AddCutToBeApplied(cutname);
    }

	void HistogramMap::RemoveCutFromHistograms(const std::string& cutname)
	{
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
		for (auto& pair : m_map)
			pair.second->FillData();
	}

	const HistogramParameters& HistogramMap::GetHistogramParams(const std::string& name)
	{
		
		auto iter = m_map.find(name);
		if (iter != m_map.end())
			return iter->second->GetParameters();
		else
			return m_nullResult;
	}

	//Only to be used within ImGui context!!
	void Navigator::HistogramMap::DrawHistograms()
	{
		for (auto& pair : m_map)
			pair.second->Draw();
	}

	//Only to be used within ImGui context!!
	void Navigator::HistogramMap::DrawHistogram(const std::string& name)
	{
		auto iter = m_map.find(name);
		if (iter != m_map.end())
			iter->second->Draw();
	}
}
