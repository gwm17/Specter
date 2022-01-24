#ifndef EDITOR_LAYER_H
#define EDITOR_LAYER_H

#include "Navigator/Layer.h"
#include "Navigator/Events/Event.h"
#include "Navigator/Events/PhysicsEvent.h"
#include "Navigator/HistogramMap.h"
#include "Navigator/CutMap.h"
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

        EventCallbackFunc m_callbackFunc;

        SpectrumPanel m_spectrumPanel;
        FileDialog m_fileDialog;
        SpectrumDialog m_spectrumDialog;
        SourceDialog m_sourceDialog;

        //ImGui Settings
        bool dockspaceOpen = true;
        bool opt_fullscreen = true;
        bool opt_fullscreen_persistant = true;
        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        bool m_removeHistogram;
        bool m_removeCut;
    };

}

#endif
