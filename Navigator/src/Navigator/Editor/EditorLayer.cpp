#include "EditorLayer.h"
#include "imgui.h"

namespace Navigator {
    
    EditorLayer::EditorLayer(HistogramMap* hmap) :
        Layer("EditorLayer"), m_histMap(hmap)
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
    }

    void EditorLayer::OnImGuiRender()
    {
        ImGui::Begin("MyTestSpace");
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
            ImGui::EndMenuBar();
        }
        
        ImGui::End();
        
    }
}
