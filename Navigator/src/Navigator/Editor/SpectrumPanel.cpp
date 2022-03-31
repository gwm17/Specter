/*
    SpectrumPanel.cpp
    This is the big boi. Renders a panel holding all of the drawn plots. Good news is that in general only a few things really require
    any modification if new types of plots are to be rendered, basically just the zoomed in spectrum rendering.

    GWM -- Feb 2022
*/
#include "SpectrumPanel.h"
#include "Navigator/Core/SpectrumManager.h"
#include "misc/cpp/imgui_stdlib.h"
#include "IconsFontAwesome5.h"

namespace Navigator {

    //Convert a StatResults struct from analysis to a std::string helper function
    std::string GenerateStatString(const std::string& name, const StatResults& results, bool is2D = true)
    {
        std::stringstream stream;
        stream << "Region: " << name << "\n" << "Integral: " << results.integral << "\n";
        if (results.integral == 0.0)
            return stream.str();
        stream << "Centroid X: " << results.cent_x << "\nStd. Dev. X: " << results.sigma_x << "\nFWHM X: " << 2.355 * results.sigma_x << "\n";
        if(is2D)
            stream << "Centroid Y: " << results.cent_y << "\nStd. Dev. Y: " << results.sigma_y << "\nFWHM Y: " << 2.355 * results.sigma_y << "\n";
        return stream.str();
    }

	SpectrumPanel::SpectrumPanel() :
        m_zoomedFlag(false), m_cutModeFlag(false), m_acceptCutFlag(false), m_zoomedGram(),  m_totalSlots(1), m_nRegions(0)
	{
        m_tableSizes[0] = 1; m_tableSizes[1] = 1;
	}

	SpectrumPanel::~SpectrumPanel() {}

    //Main render function. Handles generating subplot regions as well as the zoomed in region
	bool SpectrumPanel::OnImGuiRender(const std::vector<HistogramParameters>& histoList, const std::vector<CutParams>& cutList, const std::vector<std::string>& paramList)
	{
        static bool acceptCutFlag = false;
        m_result = false;
        if (ImGui::Begin("Active View"))
        {
            if (histoList.size() > 0)
            {
                if (m_zoomedFlag && m_zoomedGram.type != SpectrumType::None)
                {
                    if (m_zoomedGram.type == SpectrumType::Histo1D || m_zoomedGram.type == SpectrumType::Histo2D)
                    {
                        RenderCutButton();
                        ImGui::SameLine();
                    }
                    if(ImGui::Button("Clear"))
                    {
                        SpectrumManager::GetInstance().ClearHistogram(m_zoomedGram.name);
                    }
                    ImGui::SameLine();
                    RenderRemoveRegionButton();
                    if (m_zoomedGram.type == SpectrumType::Histo2D || m_zoomedGram.type == SpectrumType::Summary)
                    {
                        float* scale = SpectrumManager::GetInstance().GetColorScaleRange(m_zoomedGram.name);
                        ImGui::DragFloatRange2("Min / Max", &(scale[0]), &(scale[1]), 0.01f);
                        ImPlot::ColormapScale("##HistogramScale", scale[0], scale[1], ImVec2(0, -1), ImPlotColormap_Viridis);
                        ImGui::SameLine();
                    }
                    if (ImPlot::BeginPlot(m_zoomedGram.name.c_str(), ImVec2(-1, -1)))
                    {
                        SpectrumManager::GetInstance().DrawHistogram(m_zoomedGram.name);
                        if (!m_cutModeFlag && ImPlot::IsPlotHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                        {
                            m_zoomedFlag = false;
                            m_zoomedGram = HistogramParameters();
                        }
                        else if (m_cutModeFlag)
                        {
                            HandleCutMode();
                        }
                       
                        if (ImPlot::IsPlotSelected()) {
                            auto select = ImPlot::GetPlotSelection();
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
                                ImPlot::DragRect(int(i), &region.region.X.Min, &region.region.Y.Min, &region.region.X.Max, &region.region.Y.Max, ImVec4(1, 0, 1, 1));
                                StatResults results = SpectrumManager::GetInstance().AnalyzeHistogramRegion(m_zoomedGram.name, region.region);
                                ImPlot::PlotText(GenerateStatString(region.name, results, m_zoomedGram.y_par != "None").c_str(), (region.region.X.Max + region.region.X.Min) * 0.5, 
                                                 (region.region.Y.Min + region.region.Y.Max) * 0.5);
                            }
                        }
                        ImPlot::EndPlot();
                    }
                    RenderAcceptCutDialog();
                }
                else
                {
                    ImGui::SliderInt2("Rows, Columns", m_tableSizes, 1, 3);
                    ImGui::SameLine();
                    if (ImGui::Button("Clear All"))
                    {
                        SpectrumManager::GetInstance().ClearHistograms();
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
                                SpectrumManager::GetInstance().DrawHistogram(spec.name);
                                if (ImPlot::IsPlotHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                                {
                                    m_zoomedFlag = true;
                                    m_zoomedGram = spec;
                                }

                                for (size_t i = 0; i < m_integralRegions.size(); i++)
                                {
                                    auto& region = m_integralRegions[i];
                                    if (spec.name == region.histogram_name)
                                    {
                                        ImPlot::DragRect(int(i), &region.region.X.Min, &region.region.Y.Min, &region.region.X.Max, &region.region.Y.Max, ImVec4(1, 0, 1, 1));
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
        return m_result;
	}

    void SpectrumPanel::HandleCutMode()
    {
        switch (m_zoomedGram.type)
        {
        case SpectrumType::Histo1D:
        {
            if (m_newCutX.size() == 2)
            {
                m_acceptCutFlag = true;
            }
            else if (ImPlot::IsPlotHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                m_newCutX.push_back(ImPlot::GetPlotMousePos().x);
            }
            ImPlot::PlotVLines(m_newCutParams.name.c_str(), m_newCutX.data(), int(m_newCutX.size()));
            break;
        }
        case SpectrumType::Histo2D:
        {
            if (m_newCutX.size() >= 2 && ImPlot::IsPlotHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                m_acceptCutFlag = true;
                m_newCutX.push_back(m_newCutX[0]);
                m_newCutY.push_back(m_newCutY[0]);
            }
            else if (ImPlot::IsPlotHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                auto point = ImPlot::GetPlotMousePos();
                m_newCutX.push_back(point.x);
                m_newCutY.push_back(point.y);
            }
            ImPlot::PlotLine(m_newCutParams.name.c_str(), m_newCutX.data(), m_newCutY.data(), int(m_newCutX.size()));
            break;
        }
        }
    }

    void SpectrumPanel::RenderAcceptCutDialog()
    {
        if (m_acceptCutFlag)
        {
            m_acceptCutFlag = false;
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
                    SpectrumManager::GetInstance().AddCut(m_newCutParams, m_newCutX[0], m_newCutX[1]);
                }
                else
                {
                    SpectrumManager::GetInstance().AddCut(m_newCutParams, m_newCutX, m_newCutY);
                }
                SpectrumManager::GetInstance().AddCutToHistogramDraw(m_newCutParams.name, m_zoomedGram.name);
                ImGui::CloseCurrentPopup();
                m_result = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("No"))
            {
                ImGui::CloseCurrentPopup();
                m_result = false;
            }
            ImGui::EndPopup();
        }
    }

    //Renders Cut button as well as dialog for creating cuts.
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
            switch (m_zoomedGram.type)
            {
                case SpectrumType::Histo1D:
                {
                    m_newCutParams.type = CutType::Cut1D;
                    ImGui::BulletText("%s", ("X Parameter: " + m_newCutParams.x_par).c_str());
                    break;
                }
                case SpectrumType::Histo2D:
                {
                    m_newCutParams.type = CutType::Cut2D;
                    ImGui::BulletText("%s", ("X Parameter: " + m_newCutParams.x_par).c_str());
                    ImGui::BulletText("%s", ("Y Parameter: " + m_newCutParams.y_par).c_str());
                    break;
                }
                case SpectrumType::None: m_newCutParams.type = CutType::None; break;
                case SpectrumType::Summary: m_newCutParams.type = CutType::None; break;
            }
            ImGui::InputText("Cut Name", &m_newCutParams.name);
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

    //Simple button/dialog for removing integration regions
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
