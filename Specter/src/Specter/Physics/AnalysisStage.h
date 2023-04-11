/*
	AnalysisStage.h
	Represents a step in a chain of analyses that are run on a SpecEvent. These stages are where Parameters are set and validated. Users should use this base class
	to create their own analyses and inject them into their project. See template SpecProject for an example of use.

	GWM -- Feb 2022
*/
#ifndef ANALYSIS_STAGE_H
#define ANALYSIS_STAGE_H

#include "Specter/Core/SpecCore.h"
#include "Specter/Core/Parameter.h"
#include "SpecData.h"

namespace Specter {

	class AnalysisStage
	{
	public:
		AnalysisStage(const std::string& name="AnalysisStage");
		virtual ~AnalysisStage();

		virtual void AnalyzePhysicsEvent(const SpecEvent& event) {};

		inline std::string GetName() { return m_name; }
	private:
		std::string m_name;
	};

}

#endif