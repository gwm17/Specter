#ifndef ANALYSIS_STAGE_H
#define ANALYSIS_STAGE_H

#include "Navigator/NavCore.h"
#include "Navigator/ParameterMap.h"
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