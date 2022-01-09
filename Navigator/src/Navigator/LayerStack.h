#ifndef LAYER_STACK_H
#define LAYER_STACK_H

#include "NavCore.h"
#include "Layer.h"

namespace Navigator {

	class NAV_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void PopOverlay(Layer* layer);

		std::vector<Layer*>::iterator begin() { return m_stack.begin(); }
		std::vector<Layer*>::iterator end() { return m_stack.end(); }
	private:
		std::vector<Layer*> m_stack; //These layers are owned by the LayerStack!
		unsigned int m_insertIndex=0;
	};

}

#endif