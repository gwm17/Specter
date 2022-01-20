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

    void HistogramMap::AddCutToHistogramDraw(const std::string &cutname, const std::string &histoname)
    {
        auto iter = m_map.find(histoname);
        if(iter != m_map.end())
            iter->second->AddCutToBeDrawn(cutname);
    }

    void HistogramMap::AddCutToHistogramApplied(const std::string &cutname, const std::string &histoname)
    {
        auto iter = m_map.find(histoname);
        if(iter != m_map.end())
            iter->second->AddCutToBeApplied(cutname);
    }

	void HistogramMap::UpdateHistograms()
	{
		std::string xpar, ypar;
		ParameterMap& pmap = ParameterMap::GetInstance();
		for (auto& pair : m_map)
		{

			xpar = pair.second->GetXParam();
			ypar = pair.second->GetYParam();

			if (pair.second->Is1D())
			{
				auto iter = pmap.find(xpar);
				if (iter == pmap.end() || !iter->second->validFlag)
				{
					continue;
				}
				else
				{
					pair.second->FillData(iter->second->value);
				}
			}
			else
			{
				auto iterx = pmap.find(xpar);
				auto itery = pmap.find(ypar);
				if (iterx == pmap.end() || itery == pmap.end() || !iterx->second->validFlag || !itery->second->validFlag)
				{
					continue;
				}
				else
				{
					pair.second->FillData(iterx->second->value, itery->second->value);
				}

			}
		}
	}

	std::vector<HistogramParameters> HistogramMap::GetListOfHistogramParams()
	{
		std::vector<HistogramParameters> params;
		params.reserve(m_map.size());
		for (auto& pair : m_map)
			params.push_back(pair.second->GetParameters());
		return params;
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
