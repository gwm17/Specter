#ifndef ANALYSIS_STAGE_H
#define ANALYSIS_STAGE_H

#include "Navigator/NavCore.h"
#include "CompassHit.h"

namespace Navigator {

	class NAV_API AnalysisStage
	{
	public:
		using RawPhysicsEvent = std::vector<CompassHit>;

		AnalysisStage(const std::string& name="AnalysisStage");
		virtual ~AnalysisStage();

		virtual void AnalyzeRawPhysicsEvent(const RawPhysicsEvent& event) {};

		void AttachParameterMap() {};

		inline std::string GetName() { return m_name; }
	private:
		std::string m_name;
	};

}

#endif