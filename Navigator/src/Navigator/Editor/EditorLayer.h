/*
    EditorLayer.h
    Application layer encapsulating the editor for Navigator. Written using the Dear ImGui library. Setup based off of @TheCherno's Hazel game engine.
    EditorLayer essentially controls the state for UI related actions.

    GWM -- Feb 2022
*/
#ifndef EDITOR_LAYER_H
#define EDITOR_LAYER_H

#include "Navigator/Core/Layer.h"
#include "Navigator/Events/Event.h"
#include "Navigator/Events/PhysicsEvent.h"
#include "Navigator/Core/Histogram.h"
#include "Navigator/Core/Cut.h"
#include "SpectrumPanel.h"
#include "FileDialog.h"
#include "SpectrumDialog.h"
#include "SourceDialog.h"

namespace Navigator {

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
        virtual void OnUpdate() override;
        virtual void OnEvent(Event& event) override;


    private:
        void RemoveCutDialog();
        void RemoveHistogramDialog();
        void ExportHistogramDialog();
        void UpdateHistogramList();
        void UpdateCutList();
        void UpdateParameterList(); //Currently not really used, only once. Params all made at construction time of PhysicsLayer
        void ExportHistogram(HistogramArgs selectedGram, const std::string& filename);

        EventCallbackFunc m_callbackFunc;

        SpectrumPanel m_spectrumPanel;
        FileDialog m_fileDialog;
        SpectrumDialog m_spectrumDialog;
        SourceDialog m_sourceDialog;


        std::vector<HistogramArgs> m_histoList;
        std::vector<CutArgs> m_cutList;
        std::vector<std::string> m_paramList;

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
