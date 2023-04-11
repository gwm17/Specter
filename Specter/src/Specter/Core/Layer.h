/*
	Layer.h
	Layer is an abstract class representing an aspect of the application. Based entirely upon @TheCherno's tutorials in his game engine series. 
	
	GWM -- Feb 2022
*/
#ifndef LAYER_H
#define LAYER_H

#include "SpecCore.h"
#include "Specter/Events/Event.h"
#include "Timestep.h"

namespace Specter {

	class Layer
	{
	public:
		Layer(const std::string& name="Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnImGuiRender() {}
		virtual void OnUpdate(Timestep& step) {}
		virtual void OnEvent(Event& event) {}

		const std::string& GetName() { return m_name; }

	private:
		std::string m_name;
	};

}

#endif