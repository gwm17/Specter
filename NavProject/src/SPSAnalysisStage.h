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
		NavParameter cathode;
		NavParameter xavg_sabreCoinc;

		std::vector<NavParameter> sabre;

		//Create a few variables
		NavVariable x1_weight;
		NavVariable x2_weight;
	};

}