#include "Navigator.h"

namespace Navigator {

	class SPSAnalysisStage : public AnalysisStage
	{
	public:
		SPSAnalysisStage();
		virtual ~SPSAnalysisStage();

		virtual void AnalyzePhysicsEvent(const NavEvent& event) override;

	private:
		NavParameter delayFLTime;
		NavParameter delayFRTime;
		NavParameter delayBLTime;
		NavParameter delayBRTime;
		NavParameter x1;
		NavParameter x2;
		NavParameter xavg;

		double weight1 = 1.7;
		double weight2 = -0.7;
	};

}