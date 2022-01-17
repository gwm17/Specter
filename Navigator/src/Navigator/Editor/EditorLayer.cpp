#include "EditorLayer.h"
#include "imgui.h"
#include "implot.h"
#include "FileDialog.h"
#include "Navigator/Application.h"

namespace Navigator {
    
    EditorLayer::EditorLayer(HistogramMap* hmap) :
        Layer("EditorLayer"), m_histMap(hmap), m_spectrumPanel(hmap)
    {
    }
    
    EditorLayer::~EditorLayer() {}

    void EditorLayer::OnAttach()
    {
    }

    void EditorLayer::OnDetach()
    {
    }

    void EditorLayer::OnUpdate()
    {
    }

    void EditorLayer::OnEvent(Event& e)
    {
        EventDispatcher dispatch(e);
        dispatch.Dispatch<PhysicsParamEvent>(BIND_EVENT_FUNCTION(EditorLayer::OnPhysicsParamEvent));
    }

    void EditorLayer::UpdateHistogramLists()
    {
        m_histoList = m_histMap->GetListOfHistogramParams();
        m_spectrumPanel.UpdateActiveList(m_histoList);
    }

    void EditorLayer::UpdateCutLists()
    {
        m_cutList = CutMap::GetInstance().GetListOfCutParams();
    }

    void EditorLayer::OnImGuiRender()
    {
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
                if(ImGui::MenuItem("Open"))
                {
                    m_fileDialog.SetOpenFileDialog(true);
                }
                if(ImGui::MenuItem("Exit"))
                {
                }
                if(ImGui::MenuItem("Save"))
                {
                    m_fileDialog.SetSaveFileDialog(true);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Data Source"))
            {
                if (ImGui::MenuItem("Attach Source"))
                {
                }
                if (ImGui::MenuItem("Detach Source"))
                {
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Add"))
            {
                if (ImGui::MenuItem("Spectrum"))
                {
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Remove"))
            {
                if (ImGui::MenuItem("Spectrum"))
                {
                }
                if (ImGui::MenuItem("Cut"))
                {
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }


        std::string open_file_result = m_fileDialog.ImGuiRenderOpenFile(".nav");
        std::string save_file_result = m_fileDialog.ImGuiRenderSaveFile(".nav");
        if (!open_file_result.empty())
            NAV_INFO("Found a Open File!");
        else if (!save_file_result.empty())
            NAV_INFO("Found a Save File!");


        UpdateHistogramLists();
        UpdateCutLists();
        m_spectrumPanel.OnImGuiRender();

        if (ImGui::Begin("Spectra"))
        {
            for (auto& params : m_histoList)
            {
                if (ImGui::TreeNode(params.name.c_str()))
                {
                    ImGui::BulletText("X Parameter: %s", params.x_par.c_str());
                    ImGui::BulletText("X Bins: %d X Min: %f X Max: %f", params.nbins_x, params.min_x, params.max_x);
                    if (params.y_par != "None")
                    {
                        ImGui::BulletText("Y Parameter: %s", params.y_par.c_str());
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
            ImGui::End();
        }
        
        if(ImGui::Begin("Cuts"))
        {
            for(auto& params : m_cutList)
            {
                if(ImGui::TreeNode(params.name.c_str()))
                {
                    ImGui::BulletText("X Parameter: %s", params.x_par.c_str());
                    if(params.y_par != "None")
                        ImGui::BulletText("Y Parameter: %s", params.y_par.c_str());
                    ImGui::TreePop();
                }
            }
            ImGui::End();
        }

        ImGui::End();
    }

    bool EditorLayer::OnPhysicsParamEvent(PhysicsParamEvent& event)
    {
        NAV_INFO("{0}", event.ToString());
        m_paramList = Application::Get().GetParameterList();
        return true;
    }
}
