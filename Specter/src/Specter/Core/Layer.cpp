/*
	Layer.cpp
	Layer is an abstract class representing an aspect of the application. Based entirely upon @TheCherno's tutorials in his game engine series.

	GWM -- Feb 2022
*/
#include "Layer.h"

namespace Specter {

	Layer::Layer(const std::string& name) :
		m_name(name)
	{
	}

	Layer::~Layer() {}
}