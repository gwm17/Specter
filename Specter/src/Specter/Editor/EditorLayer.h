/*
    EditorLayer.h
    Application layer encapsulating the editor for Specter. Written using the Dear ImGui library. Setup based off of @TheCherno's Hazel game engine.
    EditorLayer essentially controls the state for UI related actions.

    GWM -- Feb 2022
*/
#ifndef EDITOR_LAYER_H
#define EDITOR_LAYER_H

#include "Specter/Core/Layer.h"
#include "Specter/Events/Event.h"
#include "Specter/Events/PhysicsEvent.h"
#include "Specter/Core/Histogram.h"
#include "Specter/Core/Cut.h"
#include "Specter/Core/Graph.h"
#include "SpectrumPanel.h"
#include "ScalerPanel.h"
#include "FileDialog.h"
#include "SpectrumDialog.h"
#include "SourceDialog.h"

namespace Specter {

    class EditorLayer : public Layer
    {
    public:
        using EventCallbackFunc = std::function<void(Event&)>;

        EditorLayer();
        ~EditorLayer();

        void SetEventCallbackFunc(const EventCallbackFunc& f) { m_callbackFunc = f; }

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnImGuiRender() override;
        virtual void OnUpdate(Timestep& step) override;
        virtual void OnEvent(Event& event) override;


    private:
        void RemoveCutDialog();
        void RemoveHistogramDialog();
        void ExportHistogramDialog();
        void UpdateHistogramList();
        void UpdateCutList();
        void UpdateParameterList(); //Currently not really used, only once. Params all made at construction time of PhysicsLayer
        void UpdateScalerList(); //Currently not really used, only once. Scalers all made at construction time of PhysicsLayer
        void UpdateGraphList(); //Same
        void ExportHistogram(HistogramArgs selectedGram, const std::string& filename);

        EventCallbackFunc m_callbackFunc;

        SpectrumPanel m_spectrumPanel;
        ScalerPanel m_scalerPanel;
        FileDialog m_fileDialog;
        SpectrumDialog m_spectrumDialog;
        SourceDialog m_sourceDialog;


        std::vector<HistogramArgs> m_histoList;
        std::vector<CutArgs> m_cutList;
        std::vector<std::string> m_paramList;
        std::vector<std::string> m_scalerList;
        std::vector<GraphArgs> m_graphList;

        //ImGui Settings
        bool dockspaceOpen = true;
        bool opt_fullscreen = true;
        bool opt_fullscreen_persistant = true;
        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        bool m_removeHistogram;
        bool m_removeCut;
        bool m_exportHistogram;
    };

    template<typename T>
    bool SortByName(const T& p1, const T& p2)
    {
        return p1.name < p2.name;
    }

}

#endif
