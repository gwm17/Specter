/*
	LayerStack.cpp
	LayerStack is a container for Layers. Should only be owned by the Application. Layers are
	managed by the LayerStack (memory-wise). There are two types of layers, overlays and regular layers.
	Overlays are processed first in the event stack. This is entirely based upon @TheCherno's work shown in his
	game engine tutorial series.

	GWM -- Feb 2022
*/
#include "LayerStack.h"

namespace Specter {

	LayerStack::LayerStack() {}

	LayerStack::~LayerStack()
	{
		for(Layer* layer : m_stack)
			delete layer;
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_stack.emplace(m_stack.begin() + m_insertIndex, layer);
		m_insertIndex++;
	}

	void LayerStack::PushOverlay(Layer* layer)
	{
		m_stack.emplace_back(layer);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto iter = std::find(m_stack.begin(), m_stack.end(), layer);
		if(iter != m_stack.end())
		{
			m_stack.erase(iter);
			m_insertIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer* layer)
	{
		auto iter = std::find(m_stack.begin(), m_stack.end(), layer);
		if(iter != m_stack.end())
			m_stack.erase(iter);
	}
}