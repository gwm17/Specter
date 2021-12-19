#include "LayerStack.h"

namespace Navigator {

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