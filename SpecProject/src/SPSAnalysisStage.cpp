/*
	SPSAnalysisStage.cpp
	Example of a user AnalysisStage. This one is based around the SE-SPS detector system in FoxLab at FSU.

	GWM -- Feb 2022
*/
#include "SPSAnalysisStage.h"

namespace Specter {

	//Construct each NavParameter with their unique name. Then bind them to the SpectrumManager.
	SPSAnalysisStage::SPSAnalysisStage(const SpectrumManager::Ref& manager) :
		AnalysisStage("SPSAnalysis"), delayFLTime("delayFLTime"), delayFRTime("delayFRTime"), delayBLTime("delayBLTime"), delayBRTime("delayBRTime"), x1("x1"), x2("x2"), xavg("xavg"),
		scintLeft("scintLeft"), anodeBack("anodeBack"), anodeFront("anodeFront"), cathode("cathode"), xavg_sabreCoinc("xavg_sabreCoinc"), x1_weight("x1_weight"), x2_weight("x2_weight"),
		beamIntegrator("beamIntegrator")
	{
		SPEC_PROFILE_FUNCTION();
		//Bind some parameters
		manager->BindParameter(delayFLTime);
		manager->BindParameter(delayFRTime);
		manager->BindParameter(delayBLTime);
		manager->BindParameter(delayBRTime);

		//Bind parameters with some default histograms. Saves us the effort of making them in the UI.
		manager->BindParameter(x1, 600, -300.0, 300.0);
		manager->BindParameter(x2, 600, -300.0, 300.0);
		manager->BindParameter(xavg, 600, -300.0, 300.0);
		manager->BindParameter(scintLeft, 4096, 0.0, 4096.0);
		manager->BindParameter(anodeBack, 4096, 0.0, 4096.0);
		manager->BindParameter(anodeFront, 4096, 0.0, 4096.0);
		manager->BindParameter(cathode, 4096, 0.0, 4096);
		manager->BindParameter(xavg_sabreCoinc, 600, -300.0, 300.0);

		//Example of injecting experiment specific info. I know that SABRE is only used in 16 channel digitizers,
		//so I can make a simple incremental id for use with the UI so that the parameter names are human readable. We map this "simple" id
		//to the more robust and universal Specter board/channel UUID using a std::unordered_map
		//This is kind of a really simple example of injecting a channel map; more complicated and advanced versions could involve reading
		//a text file to get detector ID information, detector channel number, etc.
		std::vector<std::string> sabre_list; //list of names will allow us to create a summary histogram.
		uint32_t uuid;
		for (uint32_t board = 0; board < 7; board++)
		{
			for (uint32_t channel = 0; channel < 16; channel++)
			{
				//ImGui & spdlog come prepackaged with the fmt library, so make good use of it!
				sabre_list.push_back(fmt::format("sabre_%d", board*16 + channel));
				uuid = Utilities::GetBoardChannelUUID(board, channel);
				sabre[uuid] = Parameter(sabre_list.back());
				manager->BindParameter(sabre[uuid]);
			}
			
		}

		//If you want to make a histogram default available, you can add one like this.
		manager->AddHistogramSummary(HistogramArgs("sabre_summary", "", 512, 0.0, 16384), sabre_list);
		//Note that if you save a config, the config rep of this histogram will supercede this version.

		manager->BindVariable(x1_weight);
		manager->BindVariable(x2_weight);

		manager->BindScaler(beamIntegrator);
	}

	SPSAnalysisStage::~SPSAnalysisStage() {}

	//Do some physics!
	void SPSAnalysisStage::AnalyzePhysicsEvent(const SpecEvent& event)
	{
		SPEC_PROFILE_FUNCTION();
		//You might want some flags for coincidence cases
		//Use statics to avoid allocating extra memory each call (these pipeline functions are called a lot!)
		static bool sabreFlag;

		//Most analysis stages will start kinda like this. Take the raw event data and
		//put it into Specter::Parameters using the hit id. Switches are perfect for this. Can also
		//create mapping classes to use text-file-based id association (commonly called channel maps).
		sabreFlag = false;
		for(auto& hit : event)
		{
			//Check the SABRE map first; use iterators from std::unordered_map.
			//See std::unordered_map docs for more info if this sort of idiom is unfamiliar
			auto iter = sabre.find(hit.id);
			if (iter != sabre.end())
			{
				sabreFlag = true;
				if (hit.longEnergy > iter->second.GetValue())
					iter->second.SetValue(hit.longEnergy);
				continue;
			}

			switch (hit.id)
			{
				case s_scintLeftID:
				{
					if (hit.longEnergy > scintLeft.GetValue())
						scintLeft.SetValue(hit.longEnergy);
					break;
				}
				case s_beamIntID:
				{
					beamIntegrator.Increment();
					break;
				}
				case s_cathodeID:
				{
					if (hit.longEnergy > cathode.GetValue())
						cathode.SetValue(hit.longEnergy);
					break;
				}
				case s_delayFrontLeftID:
				{
					if (!delayFLTime.IsValid())
						delayFLTime.SetValue(hit.timestamp / 1.0e3);
					break;
				}
				case s_delayFrontRightID:
				{
					if (!delayFRTime.IsValid())
						delayFRTime.SetValue(hit.timestamp / 1.0e3);
					break;
				}
				case s_delayBackLeftID:
				{
					if (!delayBLTime.IsValid())
						delayBLTime.SetValue(hit.timestamp / 1.0e3);
					break;
				}
				case s_delayBackRightID:
				{
					if (!delayBRTime.IsValid())
						delayBRTime.SetValue(hit.timestamp / 1.0e3);
					break;
				}
				case s_anodeFrontID:
				{
					if (hit.longEnergy > anodeFront.GetValue())
						anodeFront.SetValue(hit.longEnergy);
					break;
				}
				case s_anodeBackID:
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