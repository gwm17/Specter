#ifndef LAYER_H
#define LAYER_H

#include "Events/Event.h"

namespace Navigator {

	class Layer
	{
	public:
		Layer(const std::string& name="Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() { return m_name; }

	private:
		std::string m_name;
	};

}

#endif