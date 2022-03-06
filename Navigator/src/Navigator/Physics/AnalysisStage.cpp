/*
	AnalysisStage.h
	Represents a step in a chain of analyses that are run on a NavEvent. These stages are where NavParameters are set and validated. Users should use this base class
	to create their own analyses and inject them into their project. See template NavProject for an example of use.

	GWM -- Feb 2022
*/
#include "AnalysisStage.h"

namespace Navigator {

	AnalysisStage::AnalysisStage(const std::string& name) :
		m_name(name)
	{
	}

	AnalysisStage::~AnalysisStage() {}

}