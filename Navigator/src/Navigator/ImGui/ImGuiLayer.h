#ifndef IMGUI_LAYER_H
#define IMGUI_LAYER_H

#include "Navigator/NavCore.h"
#include "Navigator/Layer.h"

namespace Navigator {

	class NAV_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();

	private:
		float m_time;
	};

}

#endif