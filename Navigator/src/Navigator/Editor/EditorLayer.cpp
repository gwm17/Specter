#include "EditorLayer.h"
#include "imgui.h"
#include "implot.h"

namespace Navigator {
    
    EditorLayer::EditorLayer(HistogramMap* hmap) :
        Layer("EditorLayer"), m_histMap(hmap)
    {
        zoomFlag = false;
        zoomed_gram = "";
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
    }

    void EditorLayer::OnImGuiRender()
    {
        static bool dockspaceOpen = true;
        static bool opt_fullscreen_persistant = true;
        bool opt_fullscreen = opt_fullscreen_persistant;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
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
                }
                if(ImGui::MenuItem("Exit"))
                {
                }
                if(ImGui::MenuItem("Save"))
                {
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
                if (ImGui::MenuItem("Cut"))
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

        static std::vector<HistogramParameters> paramList = m_histMap->GetListOfHistogramParams();
        if (ImGui::Begin("Active View"))
        {
            if (paramList.size() > 0)
            {
                static std::vector<std::string> s_selectedGrams;
                static int sizes[2] = { 1,1 };
                static int total = 1;
                
                if(zoomFlag && zoomed_gram != "")
                {
                    if(ImPlot::BeginPlot(zoomed_gram.c_str(), ImVec2(-1,-1)))
                    {
                        m_histMap->DrawHistogram(zoomed_gram);
                        if (ImPlot::IsPlotHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                        {
                            NAV_INFO("We lost 'em, de-zoom and enhance!");
                            zoomFlag = false;
                            zoomed_gram = "";
                        }
                        ImPlot::EndPlot();
                    }
                }
                else
                {
                    ImGui::SliderInt2("Rows, Columns", sizes, 1, 3);
                    total = sizes[0] * sizes[1];
                    s_selectedGrams.resize(total);
                    for (auto& gram : s_selectedGrams)
                        gram = paramList[0].name;
                    if (ImGui::BeginTable("Select Histograms", sizes[1]))
                    {
                        std::string label;
                        int this_gram;
                        for (int i = 0; i < sizes[0]; i++)
                        {
                            ImGui::TableNextRow();
                            for (int j = 0; j < sizes[1]; j++)
                            {
                                ImGui::TableNextColumn();
                                this_gram = i * sizes[1] + j;
                                label = "Histogram" + std::to_string(this_gram);
                                if (ImGui::BeginCombo(label.c_str(), paramList[0].name.c_str()))
                                {
                                    for (auto& params : paramList)
                                        if (ImGui::Selectable(params.name.c_str(), params.name == s_selectedGrams[this_gram]))
                                            s_selectedGrams[this_gram] = params.name;
                                    ImGui::EndCombo();
                                }
                            }
                        }
                        ImGui::EndTable();
                    }
                    
                    if (ImPlot::BeginSubplots("Histograms", sizes[0], sizes[1], ImVec2(-1, -1)))
                    {
                        int i = 0;
                        for (auto& spec : s_selectedGrams)
                        {
                            if (ImPlot::BeginPlot(spec.c_str()))
                            {
                                m_histMap->DrawHistogram(spec);
                                if (ImPlot::IsPlotHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                                {
                                    NAV_INFO("We got'em boys, they're in plot {0}. Zoom and enhance!", i);
                                    zoomFlag = true;
                                    zoomed_gram = spec;
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
        

        if (ImGui::Begin("Spectra"))
        {
            for (auto& params : paramList)
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

                    ImGui::TreePop();
                }
            }
            ImGui::End();
        }

        ImGui::End();

       
        
    }
}
