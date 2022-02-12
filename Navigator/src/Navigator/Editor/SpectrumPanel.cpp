#include "SpectrumPanel.h"
#include "misc/cpp/imgui_stdlib.h"
#include "IconsFontAwesome5.h"

namespace Navigator {

    std::string GenerateStatString(const std::string& name, const StatResults& results, bool is2D = true)
    {
        std::stringstream stream;
        stream << "Region: " << name << "\n" << "Integral: " << results.integral << "\n";
        if (results.integral == 0)
            return stream.str();
        stream << "Centroid X: " << results.cent_x << " Std. Dev. X: " << results.sigma_x << " FWHM X: " << 2.355 * results.sigma_x << "\n";
        if(is2D)
            stream << "Centroid Y: " << results.cent_y << " Std. Dev. Y: " << results.sigma_y << " FWHM Y: " << 2.355 * results.sigma_y << "\n";
        return stream.str();
    }

	SpectrumPanel::SpectrumPanel() :
        m_zoomedFlag(false), m_cutModeFlag(false), m_zoomedGram(), m_nRegions(0), m_totalSlots(1)
	{
        m_tableSizes[0] = 1; m_tableSizes[1] = 1;
	}

	SpectrumPanel::~SpectrumPanel() {}

	bool SpectrumPanel::OnImGuiRender(const std::vector<HistogramParameters>& histoList, const std::vector<CutParams>& cutList, const std::vector<std::string>& paramList)
	{
        //HistogramMap& histMap = HistogramMap::GetInstance();
        //ParameterMap& paramMap = ParameterMap::GetInstance();
        //CutMap& cutMap = CutMap::GetInstance();
        static bool acceptCutFlag = false;
        bool result = false;
        //static std::string selectedRegion = "";
        if (ImGui::Begin("Active View"))
        {
            if (histoList.size() > 0)
            {
                if (m_zoomedFlag && m_zoomedGram.name != "")
                {
                    RenderCutButton();
                    ImGui::SameLine();
                    if(ImGui::Button("Clear"))
                    {
                        HistogramMap::GetInstance().ClearHistogram(m_zoomedGram.name);
                    }
                    ImGui::SameLine();
                    RenderRemoveRegionButton();
                    
                    if (ImPlot::BeginPlot(m_zoomedGram.name.c_str(), ImVec2(-1, -1)))
                    {
                        HistogramMap::GetInstance().DrawHistogram(m_zoomedGram.name);
                        if (!m_cutModeFlag && ImPlot::IsPlotHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                        {
                            NAV_INFO("We lost 'em, de-zoom and enhance!");
                            m_zoomedFlag = false;
                            m_zoomedGram = HistogramParameters();
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

                        if (ImPlot::IsPlotSelected()) {
                            auto& select = ImPlot::GetPlotSelection();
                            if (ImGui::IsMouseClicked(ImPlot::GetInputMap().SelectCancel)) {
                                ImPlot::CancelPlotSelection();
                                m_integralRegions.emplace_back(select, "integralRegion_"+std::to_string(m_nRegions), m_zoomedGram.name);
                                m_nRegions++;
                            }
                        }
                        for (size_t i = 0; i < m_integralRegions.size(); i++)
                        {
                            auto& region = m_integralRegions[i];
                            if (m_zoomedGram.name == region.histogram_name)
                            {
                                ImPlot::DragRect(i, &region.region.X.Min, &region.region.Y.Min, &region.region.X.Max, &region.region.Y.Max, ImVec4(1, 0, 1, 1));
                                StatResults results = HistogramMap::GetInstance().AnalyzeHistogramRegion(m_zoomedGram.name, region.region);
                                ImPlot::PlotText(GenerateStatString(region.name, results, m_zoomedGram.y_par != "None").c_str(), (region.region.X.Max + region.region.X.Min) * 0.5, 
                                                 (region.region.Y.Min + region.region.Y.Max) * 0.5);
                            }
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
                                CutMap::GetInstance().AddCut(m_newCutParams, m_newCutX[0], m_newCutX[1]);
                            }
                            else
                            {
                                CutMap::GetInstance().AddCut(m_newCutParams, m_newCutX, m_newCutY);
                            }
                            HistogramMap::GetInstance().AddCutToHistogramDraw(m_newCutParams.name, m_zoomedGram.name);
                            ImGui::CloseCurrentPopup();
                            result = true;
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("No"))
                        {
                            ImGui::CloseCurrentPopup();
                            result = false;
                        }
                        ImGui::EndPopup();
                    }
                }
                else
                {
                    ImGui::SliderInt2("Rows, Columns", m_tableSizes, 1, 3);
                    ImGui::SameLine();
                    if(ImGui::Button("Clear All"))
                    {
                        HistogramMap::GetInstance().ClearHistograms();
                    }
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
                                if (ImGui::BeginCombo(label.c_str(), m_selectedGrams[this_gram].name.c_str()))
                                {
                                    for (auto& params : histoList)
                                    {
                                        if (ImGui::Selectable(params.name.c_str(), params.name == m_selectedGrams[this_gram].name))
                                            m_selectedGrams[this_gram] = params;
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
                            if (ImPlot::BeginPlot(spec.name.c_str()))
                            {
                                HistogramMap::GetInstance().DrawHistogram(spec.name);
                                if (ImPlot::IsPlotHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                                {
                                    NAV_INFO("We got'em boys, they're in plot {0}. Zoom and enhance!", i);
                                    m_zoomedFlag = true;
                                    m_zoomedGram = spec;
                                }

                                for (size_t i = 0; i < m_integralRegions.size(); i++)
                                {
                                    auto& region = m_integralRegions[i];
                                    if (spec.name == region.histogram_name)
                                    {
                                        ImPlot::DragRect(i, &region.region.X.Min, &region.region.Y.Min, &region.region.X.Max, &region.region.Y.Max, ImVec4(1, 0, 1, 1));
                                    }
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
        return result;
	}

    void SpectrumPanel::RenderCutButton()
    {
        if (ImGui::Button(ICON_FA_CUT " Draw Cut"))
        {
            m_newCutParams = CutParams();
            m_newCutX.resize(0);
            m_newCutY.resize(0);
            ImGui::OpenPopup(ICON_FA_CUT " New Cut Dialog");
        }
        if (ImGui::BeginPopupModal(ICON_FA_CUT " New Cut Dialog"))
        {
            m_newCutParams.x_par = m_zoomedGram.x_par;
            m_newCutParams.y_par = m_zoomedGram.y_par;
            ImGui::InputText("Cut Name", &m_newCutParams.name);
            ImGui::BulletText("%s", ("X Parameter: " + m_newCutParams.x_par).c_str());
            ImGui::BulletText("%s", ("Y Parameter: " + m_newCutParams.y_par).c_str());
            if (ImGui::Button("Accept & Draw"))
            {
                m_cutModeFlag = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    void SpectrumPanel::RenderRemoveRegionButton()
    {
        static std::string selectedRegion = "";
        if (ImGui::Button("Delete Region"))
        {
            selectedRegion = "";
            ImGui::OpenPopup("Remove Integral Region");
        }
        if (ImGui::BeginPopupModal("Remove Integral Region"))
        {
            if (ImGui::BeginCombo("Region", selectedRegion.c_str()))
            {
                for (auto& region : m_integralRegions)
                {
                    if (region.histogram_name == m_zoomedGram.name)
                    {
                        if (ImGui::Selectable(region.name.c_str(), region.name == selectedRegion, ImGuiSelectableFlags_DontClosePopups))
                            selectedRegion = region.name;
                    }
                }
                ImGui::EndCombo();
            }
            if (ImGui::Button("Ok"))
            {
                RemoveSelectedRegion(selectedRegion);
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    void SpectrumPanel::RemoveSelectedRegion(const std::string& region)
    {
        for (size_t i=0; i<m_integralRegions.size(); i++)
        {
            if (m_integralRegions[i].name == region)
            {
                m_integralRegions.erase(m_integralRegions.begin() + i);
                break;
            }
        }
    }
}
