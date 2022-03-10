/*
	SPSAnalysisStage.cpp
	Example of a user AnalysisStage. This one is based around the SE-SPS detector system in FoxLab at FSU.

	GWM -- Feb 2022
*/
#include "SPSAnalysisStage.h"

namespace Navigator {

	//Construct each NavParameter with their unique name. Then bind them to the SpectrumManager.
	SPSAnalysisStage::SPSAnalysisStage() :
		AnalysisStage("SPSAnalysis"), delayFLTime("delayFLTime"), delayFRTime("delayFRTime"), delayBLTime("delayBLTime"), delayBRTime("delayBRTime"), x1("x1"), x2("x2"), xavg("xavg"),
		scintLeft("scintLeft"), anodeBack("anodeBack"), x1_weight("x1_weight"), x2_weight("x2_weight")
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

		manager.BindVariable(x1_weight);
		manager.BindVariable(x2_weight);
	}

	SPSAnalysisStage::~SPSAnalysisStage() {}

	//Do some physics!
	void SPSAnalysisStage::AnalyzePhysicsEvent(const NavEvent& event)
	{

		//Most analysis stages will start kinda like this. Take the raw event data and
		//put it into NavParameters using the hit id. Switches are perfect for this. Can also
		//create mapping classes to use text-file-based id association (commonly called channel maps).
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

		//If you want to use parameters to calculate another parameter, you
		//need to check that the parameter is valid (set in this event)!
		if(delayFLTime.IsValid() && delayFRTime.IsValid())
			x1.SetValue((delayFLTime.GetValue() - delayFRTime.GetValue())*0.5*0.4762);

		if(delayBLTime.IsValid() && delayBRTime.IsValid())
			x2.SetValue((delayBLTime.GetValue() - delayBRTime.GetValue())*0.5*0.5051);

		if (x1.IsValid() && x2.IsValid())
			xavg.SetValue(x1_weight.GetValue() * x1.GetValue() + x2_weight.GetValue() * x2.GetValue());
	}
}