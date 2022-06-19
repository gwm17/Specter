/*
	LayerStack.h
	LayerStack is a container for Layers. Should only be owned by the Application. Layers are 
	managed by the LayerStack (memory-wise). There are two types of layers, overlays and regular layers.
	Overlays are processed first in the event stack. This is entirely based upon @TheCherno's work shown in his
	game engine tutorial series.

	GWM -- Feb 2022
*/
#ifndef LAYER_STACK_H
#define LAYER_STACK_H

#include "SpecCore.h"
#include "Layer.h"

namespace Specter {

	class LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void PopOverlay(Layer* layer);

		//helpers for iterator for loops
		std::vector<Layer*>::iterator begin() { return m_stack.begin(); }
		std::vector<Layer*>::iterator end() { return m_stack.end(); }
	private:
		std::vector<Layer*> m_stack; //These layers are owned by the LayerStack!
		unsigned int m_insertIndex=0; //used to keep track of where to put layers vs. overlays.
	};

}

#endif