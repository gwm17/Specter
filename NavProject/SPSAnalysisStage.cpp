#include "SPSAnalysisStage.h"

namespace Navigator {

	SPSAnalysisStage::SPSAnalysisStage() :
		AnalysisStage("SPSAnalysis"), delayFLTime("delayFLTime"), delayFRTime("delayFRTime"), delayBLTime("delayBLTime"), delayBRTime("delayBRTime"), x1("x1"), x2("x2"), xavg("xavg"),
		scintLeft("scintLeft"), anodeBack("anodeBack")
	{
		SpectrumManager& manager = SpectrumManager::GetInstance();
		manager.BindParameter(delayFLTime);
		manager.BindParameter(delayFRTime);
		manager.BindParameter(delayBLTime);
		manager.BindParameter(delayBRTime);
		manager.BindParameter(x1);
		manager.BindParameter(x2);
		manager.BindParameter(xavg);
		manager.BindParameter(scintLeft);
		manager.BindParameter(anodeBack);
	}

	SPSAnalysisStage::~SPSAnalysisStage() {}

	void SPSAnalysisStage::AnalyzePhysicsEvent(const NavEvent& event)
	{


		for(auto& hit : event)
		{
			switch (hit.id)
			{
				case 129:
					scintLeft.SetValue(hit.longEnergy);
					break;
				case 136:
					delayFLTime.SetValue(hit.timestamp / 1.0e3);
					break;
				case 137:
					delayFRTime.SetValue(hit.timestamp / 1.0e3);
					break;
				case 138:
					delayBLTime.SetValue(hit.timestamp / 1.0e3);
					break;
				case 139:
					delayBRTime.SetValue(hit.timestamp / 1.0e3);
					break;
				case 143:
					anodeBack.SetValue(hit.longEnergy);
					break;
			}
		}

		if(delayFLTime.IsValid() && delayFRTime.IsValid())
			x1.SetValue((delayFLTime.GetValue() - delayFRTime.GetValue())*0.5);

		if(delayBLTime.IsValid() && delayBRTime.IsValid())
			x2.SetValue((delayBLTime.GetValue() - delayBRTime.GetValue())*0.5);
	}
}