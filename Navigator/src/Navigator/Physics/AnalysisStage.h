/*
	AnalysisStage.h
	Represents a step in a chain of analyses that are run on a NavEvent. These stages are where NavParameters are set and validated. Users should use this base class
	to create their own analyses and inject them into their project. See template NavProject for an example of use.

	GWM -- Feb 2022
*/
#ifndef ANALYSIS_STAGE_H
#define ANALYSIS_STAGE_H

#include "Navigator/Core/NavCore.h"
#include "Navigator/Core/Parameter.h"
#include "NavData.h"

namespace Navigator {

	class NAV_API AnalysisStage
	{
	public:
		AnalysisStage(const std::string& name="AnalysisStage");
		virtual ~AnalysisStage();

		virtual void AnalyzePhysicsEvent(const NavEvent& event) {};

		inline std::string GetName() { return m_name; }
	private:
		std::string m_name;
	};

}

#endif