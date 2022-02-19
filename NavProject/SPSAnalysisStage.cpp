#include "SPSAnalysisStage.h"

namespace Navigator {

	SPSAnalysisStage::SPSAnalysisStage() :
		AnalysisStage("SPSAnalysis"), delayFLTime("delayFLTime"), delayFRTime("delayFRTime"), delayBLTime("delayBLTime"), delayBRTime("delayBRTime"), x1("x1"), x2("x2"), xavg("xavg")
	{
	}

	SPSAnalysisStage::~SPSAnalysisStage() {}

	void SPSAnalysisStage::AnalyzePhysicsEvent(const NavEvent& event)
	{


		for(auto& hit : event)
		{
			if(hit.id == 136)
				delayFLTime.SetValue(hit.timestamp/1.0e3);
			else if(hit.id == 137)
				delayFRTime.SetValue(hit.timestamp/1.0e3);
			else if(hit.id == 138)
				delayBLTime.SetValue(hit.timestamp/1.0e3);
			else if(hit.id == 139)
				delayBRTime.SetValue(hit.timestamp/1.0e3);
		}

		if(delayFLTime.IsValid() && delayFRTime.IsValid())
			x1.SetValue((delayFLTime.GetValue() - delayFRTime.GetValue())*0.5);

		if(delayBLTime.IsValid() && delayBRTime.IsValid())
			x1.SetValue((delayBLTime.GetValue() - delayBRTime.GetValue())*0.5);
	}
}