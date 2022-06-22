/*
	SPSAnalysisStage.h
	Example of a user AnalysisStage. This one is based around the SE-SPS detector system in FoxLab at FSU.

	GWM -- Feb 2022
*/
#include "Specter.h"

namespace Specter {

	class SPSAnalysisStage : public AnalysisStage
	{
	public:
		SPSAnalysisStage();
		virtual ~SPSAnalysisStage();

		virtual void AnalyzePhysicsEvent(const SpecEvent& event) override;

	private:
		//Create a bunch of parameters
		Parameter delayFLTime;
		Parameter delayFRTime;
		Parameter delayBLTime;
		Parameter delayBRTime;
		Parameter x1;
		Parameter x2;
		Parameter xavg;
		Parameter scintLeft;
		Parameter anodeBack;
		Parameter anodeFront;
		Parameter cathode;
		Parameter xavg_sabreCoinc;

		std::vector<Parameter> sabre;

		//Create a few variables
		Variable x1_weight;
		Variable x2_weight;

		//Create a scaler
		Scaler beamIntegrator;
	};

}