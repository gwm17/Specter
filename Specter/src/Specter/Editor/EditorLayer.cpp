/*
    EditorLayer.cpp
    Application layer encapsulating the editor for Specter. Written using the Dear ImGui library. Setup based off of @TheCherno's Hazel game engine.
    EditorLayer essentially controls the state for UI related actions.

    GWM -- Feb 2022
*/
#include "EditorLayer.h"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "implot.h"
#include "FileDialog.h"
#include "Specter/Core/Application.h"
#include "Specter/Core/SpectrumSerializer.h"

#include "IconsFontAwesome5.h"

namespace Specter {

    bool SortByString(const std::string& p1, const std::string& p2)
    {
        return p1 < p2;
    }

    EditorLayer::EditorLayer(const SpectrumManager::Ref& manager) :
        Layer("EditorLayer"), m_manager(manager), m_removeHistogram(false), m_removeCut(false), m_exportHistogram(false)
    {
    }
    
    EditorLayer::~EditorLayer() {}

    void EditorLayer::OnAttach()
    {
    }

    void EditorLayer::OnDetach()
    {
    }

    void EditorLayer::OnUpdate(Timestep& step)
    {
        m_manager->UpdateGraphs(step);
    }

    void EditorLayer::OnEvent(Event& e)
    {
    }

    //These updates are used whenever a new object is added to the manager.
    void EditorLayer::UpdateHistogramList()
    {
        m_histoList = m_manager->GetListOfHistograms();
        std::sort(m_histoList.begin(), m_histoList.end(), SortByName<HistogramArgs>);
    }

    void EditorLayer::UpdateCutList()
    {
        m_cutList = m_manager->GetListOfCuts();
        std::sort(m_cutList.begin(), m_cutList.end(), SortByName<CutArgs>);
    }

    void EditorLayer::UpdateParameterList()
    {
        m_paramList = m_manager->GetListOfParameters();
        std::sort(m_paramList.begin(), m_paramList.end(), SortByString);
    }

    void EditorLayer::UpdateScalerList()
    {
        m_scalerList = m_manager->GetListOfScalers();
        std::sort(m_scalerList.begin(), m_scalerList.end(), SortByString);
    }

    void EditorLayer::UpdateGraphList()
    {
        m_graphList = m_manager->GetListOfGraphs();
        std::sort(m_graphList.begin(), m_graphList.end(), SortByName<GraphArgs>);
    }

    //The main function
    void EditorLayer::OnImGuiRender()
    {
        SPEC_PROFILE_FUNCTION();
        static bool startFlag = true; //first render retrieve base 
        if(startFlag)
        {
            UpdateParameterList();
            UpdateHistogramList();
            UpdateCutList();
            UpdateScalerList();
            UpdateGraphList();
            startFlag = false;
        }
        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        if (opt_fullscreen)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("MyTestSpace", &dockspaceOpen, window_flags);
        ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // DockSpace
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        float minWinSizeX = style.WindowMinSize.x;
        style.WindowMinSize.x = 370.0f;
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        style.WindowMinSize.x = minWinSizeX;

        if(ImGui::BeginMenuBar())
        {
            if(ImGui::BeginMenu("File"))
            {
                if(ImGui::MenuItem(ICON_FA_FOLDER_OPEN "\tOpen"))
                {
                    m_fileDialog.OpenDialog(FileDialog::Type::OpenFile);
                }
                if(ImGui::MenuItem(ICON_FA_SAVE "\tSave"))
                {
                    m_fileDialog.OpenDialog(FileDialog::Type::SaveFile);
                }
                if (ImGui::MenuItem(ICON_FA_TIMES_CIRCLE "\tExit"))
                {
                    Application::Get().Close();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Data Source"))
            {
                if (ImGui::MenuItem(ICON_FA_LINK "\tAttach Source"))
                {
                    m_sourceDialog.OpenSourceDialog();
                }
                if (ImGui::MenuItem(ICON_FA_UNLINK "\tDetach Source"))
                {
                    PhysicsStopEvent event;
                    m_callbackFunc(event);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Add"))
            {
                if (ImGui::MenuItem(ICON_FA_CHART_BAR "\tSpectrum"))
                {
                    m_spectrumDialog.SetSpectrumDialog();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Remove"))
            {
                if (ImGui::MenuItem(ICON_FA_CHART_BAR "\tSpectrum"))
                {
                    m_removeHistogram = true;
                }
                if (ImGui::MenuItem(ICON_FA_CUT "\tCut"))
                {
                    m_removeCut = true;
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Export"))
            {
                if (ImGui::MenuItem(ICON_FA_SAVE "\tAs .csv"))
                {
                    m_exportHistogram = true;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        //Render all of our sub-windows, dialogs, panels, etc
        auto fd_result = m_fileDialog.RenderFileDialog(".spec");
        if (!fd_result.first.empty())
        {
            switch (fd_result.second)
            {
                case FileDialog::Type::OpenFile:
                {
                    SpectrumSerializer serializer(fd_result.first);
                    serializer.DeserializeData(m_manager);
                    UpdateHistogramList();
                    UpdateCutList();
                    break;
                }
                case FileDialog::Type::SaveFile:
                {
                    SPEC_INFO("Found a Save File! {0}", fd_result.first);
                    SpectrumSerializer serializer(fd_result.first);
                    serializer.SerializeData(m_manager, m_histoList, m_cutList);
                    break;
                }
            }
        }
        
        
        if(m_spectrumDialog.ImGuiRenderSpectrumDialog(m_manager, m_histoList, m_cutList, m_paramList))
            UpdateHistogramList();

        m_scalerPanel.OnImGuiRender(m_manager, m_scalerList, m_graphList);

        m_sourceDialog.ImGuiRenderSourceDialog();

        RemoveHistogramDialog();

        RemoveCutDialog();
        
        ExportHistogramDialog();

        if(m_spectrumPanel.OnImGuiRender(m_manager, m_histoList, m_cutList, m_paramList))
        {
            UpdateCutList();
            UpdateHistogramList();
        }

        if (ImGui::Begin(ICON_FA_CHART_BAR " Spectra"))
        {
            for (auto& params : m_histoList)
            {
                if (ImGui::TreeNode(params.name.c_str()))
                {
                    ImGui::BulletText("%s", ("X Parameter: "+params.x_par).c_str());
                    ImGui::BulletText("X Bins: %d X Min: %f X Max: %f", params.nbins_x, params.min_x, params.max_x);
                    if (params.y_par != "None")
                    {
                        ImGui::BulletText("%s", ("Y Parameter: "+params.y_par).c_str());
                        ImGui::BulletText("Y Bins: %d Y Min: %f Y Max: %f", params.nbins_y, params.min_y, params.max_y);
                    }
                    if(params.cutsDrawnUpon.size() != 0 && ImGui::TreeNode("Cuts Drawn"))
                    {
                        for(auto& cut : params.cutsDrawnUpon)
                            ImGui::BulletText("%s", cut.c_str());
                        ImGui::TreePop();
                    }
                    if(params.cutsAppliedTo.size() != 0 && ImGui::TreeNode("Cuts Applied"))
                    {
                        for(auto& cut : params.cutsAppliedTo)
                            ImGui::BulletText("%s", cut.c_str());
                        ImGui::TreePop();
                    }
                    ImGui::TreePop();
                }
            }
        }
        ImGui::End();
        
        if(ImGui::Begin(ICON_FA_CUT " Cuts"))
        {
            for(auto& params : m_cutList)
            {
                if(ImGui::TreeNode(params.name.c_str()))
                {
                    ImGui::BulletText("%s", ("X Parameter: "+params.x_par).c_str());
                    if(params.y_par != "None")
                        ImGui::BulletText("%s", ("Y Parameter: "+params.y_par).c_str());
                    ImGui::TreePop();
                }
            }
        }
        ImGui::End();

        ImGui::End();
    }

    //Simple dialogs, no need for separate class

    void EditorLayer::RemoveHistogramDialog()
    {
        SPEC_PROFILE_FUNCTION();
        static std::string selectedGram = "";
        if (m_removeHistogram)
        {
            selectedGram = "";
            m_removeHistogram = false;
            ImGui::OpenPopup("Remove Histogram");
        }
        if (ImGui::BeginPopupModal("Remove Histogram"))
        {
            if (ImGui::BeginCombo("Histogram", selectedGram.c_str()))
            {
                for (auto& gram : m_histoList)
                {
                    if (ImGui::Selectable(gram.name.c_str(), gram.name == selectedGram, ImGuiSelectableFlags_DontClosePopups))
                        selectedGram = gram.name;
                }
                ImGui::EndCombo();
            }
            if (ImGui::Button("Ok"))
            {
                m_manager->RemoveHistogram(selectedGram);
                UpdateHistogramList();
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

    void EditorLayer::RemoveCutDialog()
    {
        SPEC_PROFILE_FUNCTION();
        static std::string selectedCut = "";
        if (m_removeCut)
        {
            selectedCut = "";
            m_removeCut = false;
            ImGui::OpenPopup("Remove Cut");
        }
        if (ImGui::BeginPopupModal("Remove Cut"))
        {
            if (ImGui::BeginCombo("Cut", selectedCut.c_str()))
            {
                for (auto& cut : m_cutList)
                {
                    if (ImGui::Selectable(cut.name.c_str(), cut.name == selectedCut, ImGuiSelectableFlags_DontClosePopups))
                        selectedCut = cut.name;
                }
                ImGui::EndCombo();
            }
            if (ImGui::Button("Ok"))
            {
                m_manager->RemoveCut(selectedCut);
                UpdateHistogramList();
                UpdateCutList();
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

    void EditorLayer::ExportHistogramDialog()
    {
        SPEC_PROFILE_FUNCTION();
        static std::string filename = "";
        static HistogramArgs selectedGram = HistogramArgs();
        if(m_exportHistogram)
        {
            filename = "";
            selectedGram = HistogramArgs();
            m_exportHistogram = false;
            ImGui::OpenPopup("Export Histogram");
        }
        if(ImGui::BeginPopupModal("Export Histogram"))
        {
            if(ImGui::BeginCombo("Histogram", selectedGram.name.c_str()))
            {
                for (auto& gram : m_histoList)
                {
                    if (ImGui::Selectable(gram.name.c_str(), gram.name == selectedGram.name, ImGuiSelectableFlags_DontClosePopups))
                        selectedGram = gram;
                }
                ImGui::EndCombo();
            }
            ImGui::InputText("File", &filename);
            ImGui::SameLine();
            if(ImGui::Button("Open"))
            {
                m_fileDialog.OpenDialog(FileDialog::Type::SaveFile);
            }
            auto result = m_fileDialog.RenderFileDialog(".csv");
            if(!result.first.empty() && result.second == FileDialog::Type::SaveFile)
                filename = result.first;
            if(ImGui::Button("Ok"))
            {
                ExportHistogram(selectedGram, filename);
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if(ImGui::Button("Cancel"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
    }

    void EditorLayer::ExportHistogram(HistogramArgs selectedGram, const std::string& filename)
    {
        std::ofstream output(filename);
        if(!output.is_open())
        {
            SPEC_ERROR("Unable to create export file {0}. Check pathing.", filename);
            return;
        }
        
        std::vector<double> data = m_manager->GetBinData(selectedGram.name);
        
        output<<"Histogram Name,"<<selectedGram.name<<std::endl;
        output<<"Min X,"<<selectedGram.min_x<<std::endl<<"Max X,"<<selectedGram.max_x<<std::endl;
        if(selectedGram.y_par != "None")
            output<<"Min Y,"<<selectedGram.min_y<<std::endl<<"Max Y,"<<selectedGram.max_y<<std::endl;
        output<<"Nbins,"<<data.size()<<std::endl;
        output<<"Bin,Counts"<<std::endl;
        for(size_t i=0; i<data.size(); i++)
            output<<i<<","<<data[i]<<std::endl;
        output.close();
    }
}
