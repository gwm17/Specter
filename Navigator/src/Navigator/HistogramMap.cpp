#include "HistogramMap.h"
#include "ParameterMap.h"

namespace Navigator {
	Navigator::HistogramMap::HistogramMap()
	{
	}

	Navigator::HistogramMap::~HistogramMap()
	{
	}

	void Navigator::HistogramMap::UpdateHistograms()
	{
		std::lock_guard<std::mutex> guard(m_histMutex);
		std::string xpar, ypar;
		ParameterMap& pmap = ParameterMap::GetInstance();
		for (auto& pair : m_map)
		{
			xpar = pair.second->GetXParam();
			ypar = pair.second->GetYParam();

			if (ypar == "None")
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
					continue;
				else
				{
					pair.second->FillData(iterx->second->value, itery->second->value);
				}

			}
		}
	}

	//Only to be used within ImGui context!!
	void Navigator::HistogramMap::DrawHistograms()
	{
		std::lock_guard<std::mutex> guard(m_histMutex);
		for (auto& pair : m_map)
			pair.second->Draw();
	}
}