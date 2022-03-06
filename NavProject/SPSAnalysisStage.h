/*
	SPSAnalysisStage.h
	Example of a user AnalysisStage. This one is based around the SE-SPS detector system in FoxLab at FSU.

	GWM -- Feb 2022
*/
#include "Navigator.h"

namespace Navigator {

	class SPSAnalysisStage : public AnalysisStage
	{
	public:
		SPSAnalysisStage();
		virtual ~SPSAnalysisStage();

		virtual void AnalyzePhysicsEvent(const NavEvent& event) override;

	private:
		//Create a bunch of parameters
		NavParameter delayFLTime;
		NavParameter delayFRTime;
		NavParameter delayBLTime;
		NavParameter delayBRTime;
		NavParameter x1;
		NavParameter x2;
		NavParameter xavg;
		NavParameter scintLeft;
		NavParameter anodeBack;

		//some variables.
		double weight1 = 1.7;
		double weight2 = -0.7;
	};

}