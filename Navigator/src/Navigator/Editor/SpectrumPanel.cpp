#include "SpectrumPanel.h"
#include "implot.h"
#include "misc/cpp/imgui_stdlib.h"
#include "IconsFontAwesome5.h"

namespace Navigator {

	SpectrumPanel::SpectrumPanel() :
        m_zoomedFlag(false), m_cutModeFlag(false), m_zoomedGram(""), m_totalSlots(1)
	{
        m_tableSizes[0] = 1; m_tableSizes[1] = 1;
	}

	SpectrumPanel::~SpectrumPanel() {}

	void SpectrumPanel::OnImGuiRender()
	{
        HistogramMap& histMap = HistogramMap::GetInstance();
        ParameterMap& paramMap = ParameterMap::GetInstance();
        CutMap& cutMap = CutMap::GetInstance();
        static bool acceptCutFlag = false;
        if (ImGui::Begin("Active View"))
        {
            if (histMap.size() > 0)
            {
                if (m_zoomedFlag && m_zoomedGram != "")
                {
                    if(ImGui::Button(ICON_FA_CUT " Draw Cut"))
                    {
                        m_newCutParams = CutParams();
                        m_newCutX.resize(0);
                        m_newCutY.resize(0);
                        ImGui::OpenPopup(ICON_FA_CUT " New Cut Dialog");
                    }
                    if(ImGui::BeginPopupModal(ICON_FA_CUT " New Cut Dialog"))
                    {
                        auto& zoomed_params = histMap.GetHistogramParams(m_zoomedGram);
                        m_newCutParams.x_par = zoomed_params.x_par;
                        m_newCutParams.y_par = zoomed_params.y_par;
                        ImGui::InputText("Cut Name", &m_newCutParams.name);
                        ImGui::BulletText(("X Parameter: "+m_newCutParams.x_par).c_str());
                        ImGui::BulletText(("Y Parameter: "+m_newCutParams.y_par).c_str());
                        if(ImGui::Button("Accept & Draw"))
                        {
                            m_cutModeFlag = true;
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SameLine();
                        if(ImGui::Button("Cancel"))
                        {
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                    
                    if (ImPlot::BeginPlot(m_zoomedGram.c_str(), ImVec2(-1, -1)))
                    {
                        histMap.DrawHistogram(m_zoomedGram);
                        if (!m_cutModeFlag && ImPlot::IsPlotHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                        {
                            NAV_INFO("We lost 'em, de-zoom and enhance!");
                            m_zoomedFlag = false;
                            m_zoomedGram = "";
                        }
                        else if (m_cutModeFlag && m_newCutParams.y_par == "None")
                        {
                            if (m_newCutX.size() == 2)
                            {
                                acceptCutFlag = true;
                            }
                            else if (ImPlot::IsPlotHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                            {
                                m_newCutX.push_back(ImPlot::GetPlotMousePos().x);
                            }
                            ImPlot::PlotVLines(m_newCutParams.name.c_str(), m_newCutX.data(), m_newCutX.size());
                           
                        }
                        else if(m_cutModeFlag)
                        {
                            if (m_newCutX.size() >= 2 && ImPlot::IsPlotHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                            {
                                acceptCutFlag = true;
                                m_newCutX.push_back(m_newCutX[0]);
                                m_newCutY.push_back(m_newCutY[0]);
                            }
                            else if (ImPlot::IsPlotHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                            {
                                auto point = ImPlot::GetPlotMousePos();
                                m_newCutX.push_back(point.x);
                                m_newCutY.push_back(point.y);
                            }
                            ImPlot::PlotLine(m_newCutParams.name.c_str(), m_newCutX.data(), m_newCutY.data(), m_newCutX.size());
                        }
                        ImPlot::EndPlot();
                    }
                    if (acceptCutFlag)
                    {
                        acceptCutFlag = false;
                        m_cutModeFlag = false;
                        ImGui::OpenPopup("Accept Cut");
                    }
                    if (ImGui::BeginPopupModal("Accept Cut"))
                    {
                        ImGui::Text("Save this Cut?");
                        if (ImGui::Button("Yes"))
                        {
                            if (m_newCutParams.y_par == "None")
                            {
                                std::sort(m_newCutX.begin(), m_newCutX.end());
                                cutMap.AddCut(m_newCutParams, m_newCutX[0], m_newCutX[1]);
                            }
                            else
                            {
                                cutMap.AddCut(m_newCutParams, m_newCutX, m_newCutY);
                            }
                            histMap.AddCutToHistogramDraw(m_newCutParams.name, m_zoomedGram);
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("No"))
                        {
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
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
