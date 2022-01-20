#include "SpectrumPanel.h"
#include "implot.h"

namespace Navigator {

	SpectrumPanel::SpectrumPanel() :
        m_zoomedFlag(false), m_zoomedGram(""), m_totalSlots(1)
	{
        m_tableSizes[0] = 1; m_tableSizes[1] = 1;
	}

	SpectrumPanel::~SpectrumPanel() {}

	void SpectrumPanel::OnImGuiRender()
	{
        HistogramMap& histMap = HistogramMap::GetInstance();
        if (ImGui::Begin("Active View"))
        {
            if (histMap.size() > 0)
            {
                if (m_zoomedFlag && m_zoomedGram != "")
                {
                    if (ImPlot::BeginPlot(m_zoomedGram.c_str(), ImVec2(-1, -1)))
                    {
                        histMap.DrawHistogram(m_zoomedGram);
                        if (ImPlot::IsPlotHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                        {
                            NAV_INFO("We lost 'em, de-zoom and enhance!");
                            m_zoomedFlag = false;
                            m_zoomedGram = "";
                        }
                        ImPlot::EndPlot();
                    }
                }
                else
                {
                    ImGui::SliderInt2("Rows, Columns", m_tableSizes, 1, 3);
                    m_totalSlots = m_tableSizes[0] * m_tableSizes[1];
                    m_selectedGrams.resize(m_totalSlots);
                    if (ImGui::BeginTable("Select Histograms", m_tableSizes[1]))
                    {
                        std::string label;
                        int this_gram;
                        for (int i = 0; i < m_tableSizes[0]; i++)
                        {
                            ImGui::TableNextRow();
                            for (int j = 0; j < m_tableSizes[1]; j++)
                            {
                                ImGui::TableNextColumn();
                                this_gram = i * m_tableSizes[1] + j;
                                label = "Histogram" + std::to_string(this_gram);
                                if (ImGui::BeginCombo(label.c_str(), m_selectedGrams[this_gram].c_str()))
                                {
                                    for (auto& gram : histMap)
                                    {
                                        auto& params = gram.second->GetParameters();
                                        if (ImGui::Selectable(params.name.c_str(), params.name == m_selectedGrams[this_gram]))
                                            m_selectedGrams[this_gram] = params.name;
                                    }
                                    ImGui::EndCombo();
                                }
                            }
                        }
                        ImGui::EndTable();
                    }

                    if (ImPlot::BeginSubplots("Histograms", m_tableSizes[0], m_tableSizes[1], ImVec2(-1, -1)))
                    {
                        int i = 0;
                        for (auto& spec : m_selectedGrams)
                        {
                            if (ImPlot::BeginPlot(spec.c_str()))
                            {
                                histMap.DrawHistogram(spec);
                                if (ImPlot::IsPlotHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                                {
                                    NAV_INFO("We got'em boys, they're in plot {0}. Zoom and enhance!", i);
                                    m_zoomedFlag = true;
                                    m_zoomedGram = spec;
                                }
                                ImPlot::EndPlot();
                            }
                            i++;
                        }
                        ImPlot::EndSubplots();
                    }
                }
            }
            ImGui::End();
        }
	}
}