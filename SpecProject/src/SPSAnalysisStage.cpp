/*
	SPSAnalysisStage.cpp
	Example of a user AnalysisStage. This one is based around the SE-SPS detector system in FoxLab at FSU.

	GWM -- Feb 2022
*/
#include "SPSAnalysisStage.h"

namespace Specter {

	//Construct each NavParameter with their unique name. Then bind them to the SpectrumManager.
	SPSAnalysisStage::SPSAnalysisStage() :
		AnalysisStage("SPSAnalysis"), delayFLTime("delayFLTime"), delayFRTime("delayFRTime"), delayBLTime("delayBLTime"), delayBRTime("delayBRTime"), x1("x1"), x2("x2"), xavg("xavg"),
		scintLeft("scintLeft"), anodeBack("anodeBack"), anodeFront("anodeFront"), cathode("cathode"), xavg_sabreCoinc("xavg_sabreCoinc"), x1_weight("x1_weight"), x2_weight("x2_weight")
	{
		SPEC_PROFILE_FUNCTION();
		SpectrumManager& manager = SpectrumManager::GetInstance();
		manager.BindParameter(delayFLTime);
		manager.BindParameter(delayFRTime);
		manager.BindParameter(delayBLTime);
		manager.BindParameter(delayBRTime);
		//Bind parameters with some default histograms. Saves us the effort of making them in the UI.
		manager.BindParameter(x1, 600, -300.0, 300.0);
		manager.BindParameter(x2, 600, -300.0, 300.0);
		manager.BindParameter(xavg, 600, -300.0, 300.0);
		manager.BindParameter(scintLeft, 4096, 0.0, 4096.0);
		manager.BindParameter(anodeBack, 4096, 0.0, 4096.0);
		manager.BindParameter(anodeFront, 4096, 0.0, 4096.0);
		manager.BindParameter(cathode, 4096, 0.0, 4096);
		manager.BindParameter(xavg_sabreCoinc, 600, -300.0, 300.0);

		std::vector<std::string> sabre_list;
		for (int i = 0; i < 127; i++)
		{
			sabre_list.push_back("sabre_" + std::to_string(i));
			sabre.emplace_back(sabre_list[i]);
			manager.BindParameter(sabre[i]);
		}

		//If you want to make a histogram default available, you can add one like this.
		manager.AddHistogramSummary(HistogramArgs("sabre_summary", "", 512, 0.0, 16384), sabre_list);
		//Note that if you save a config, the config rep of this histogram will supercede this version.

		manager.BindVariable(x1_weight);
		manager.BindVariable(x2_weight);
	}

	SPSAnalysisStage::~SPSAnalysisStage() {}

	//Do some physics!
	void SPSAnalysisStage::AnalyzePhysicsEvent(const SpecEvent& event)
	{

		SPEC_PROFILE_FUNCTION();
		//Most analysis stages will start kinda like this. Take the raw event data and
		//put it into NavParameters using the hit id. Switches are perfect for this. Can also
		//create mapping classes to use text-file-based id association (commonly called channel maps).
		bool sabreFlag = false;
		for(auto& hit : event)
		{
			if (hit.id < 127)
			{
				sabreFlag = true;
				if (hit.longEnergy > sabre[hit.id].GetValue())
					sabre[hit.id].SetValue(hit.longEnergy);
			}
			switch (hit.id)
			{
				case 129:
				{
					if (hit.longEnergy > scintLeft.GetValue())
						scintLeft.SetValue(hit.longEnergy);
					break;
				}
				case 135:
				{
					if (hit.longEnergy > cathode.GetValue())
						cathode.SetValue(hit.longEnergy);
					break;
				}
				case 136:
				{
					if (!delayFLTime.IsValid())
						delayFLTime.SetValue(hit.timestamp / 1.0e3);
					break;
				}
				case 137:
				{
					if (!delayFRTime.IsValid())
						delayFRTime.SetValue(hit.timestamp / 1.0e3);
					break;
				}
				case 138:
				{
					if (!delayBLTime.IsValid())
						delayBLTime.SetValue(hit.timestamp / 1.0e3);
					break;
				}
				case 139:
				{
					if (!delayBRTime.IsValid())
						delayBRTime.SetValue(hit.timestamp / 1.0e3);
					break;
				}
				case 141:
				{
					if (hit.longEnergy > anodeFront.GetValue())
						anodeFront.SetValue(hit.longEnergy);
					break;
				}
				case 143:
				{
					if (hit.longEnergy > anodeBack.GetValue())
						anodeBack.SetValue(hit.longEnergy);
					break;
				}
			}
		}

		//If you want to use parameters to calculate another parameter, you
		//need to check that the parameter is valid (set in this event)!
		if(delayFLTime.IsValid() && delayFRTime.IsValid())
			x1.SetValue((delayFLTime.GetValue() - delayFRTime.GetValue())*0.5*0.4762);

		if(delayBLTime.IsValid() && delayBRTime.IsValid())
			x2.SetValue((delayBLTime.GetValue() - delayBRTime.GetValue())*0.5*0.5051);

		if (x1.IsValid() && x2.IsValid())
		{
			xavg.SetValue(x1_weight.GetValue() * x1.GetValue() + x2_weight.GetValue() * x2.GetValue());
			if (sabreFlag)
				xavg_sabreCoinc.SetValue(xavg.GetValue());
		}
	}
}