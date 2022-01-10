#ifndef EDITOR_LAYER_H
#define EDITOR_LAYER_H

#include "Navigator/Layer.h"
#include "Navigator/Events/Event.h"
#include "Navigator/HistogramMap.h"

namespace Navigator {

    class EditorLayer : public Layer
    {
    public:
        EditorLayer(HistogramMap* hmap);
        ~EditorLayer();
        
        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnImGuiRender() override;
        virtual void OnUpdate() override;
        virtual void OnEvent(Event& event) override;
        
    private:
        HistogramMap* m_histMap; //Not owned by the EditorLayer!!
    };

}

#endif
