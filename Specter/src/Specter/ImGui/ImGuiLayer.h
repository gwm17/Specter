/*
	ImGuiLayer.h
	The layer containing all of the ImGui related setup and calls. Based on the work by @TheCherno in his game engine series.
	Should always exist as an overlay in the Application LayerStack. Note that it currently is OpenGL specific based on
	ImGui implementation/backends.

	GWM -- Feb 2022
*/
#ifndef IMGUI_LAYER_H
#define IMGUI_LAYER_H

#include "Specter/Core/SpecCore.h"
#include "Specter/Core/Layer.h"

namespace Specter {

	class ImGuiLayer : public Layer
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