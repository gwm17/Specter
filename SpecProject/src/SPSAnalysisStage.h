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
		SPSAnalysisStage(const SpectrumManager::Ref& manager);
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

		//For collections of parameters, prefer std::unordered_map or std::map over vector
		//Better with use of UUIDs for board-channel pairs
		std::unordered_map<uint32_t, Parameter> sabre;

		//Create a few variables
		Variable x1_weight;
		Variable x2_weight;

		//Create a scaler
		Scaler beamIntegrator;

		//Define some board-channel ID's that we'll use. The static keyword means that we only need to calculate them once,
		//constexpr allows us to use them in a switch (and makes them compile time evaluated)
		//Note that to make them static constexpr we use literals (i.e. hardcoded) in the arguments
		static constexpr uint32_t s_scintLeftID       = Utilities::GetBoardChannelUUID(8, 0);
		static constexpr uint32_t s_beamIntID         = Utilities::GetBoardChannelUUID(8, 5);
		static constexpr uint32_t s_cathodeID         = Utilities::GetBoardChannelUUID(8, 7);
		static constexpr uint32_t s_delayFrontLeftID  = Utilities::GetBoardChannelUUID(8, 8);
		static constexpr uint32_t s_delayFrontRightID = Utilities::GetBoardChannelUUID(8, 9);
		static constexpr uint32_t s_delayBackLeftID   = Utilities::GetBoardChannelUUID(8, 10);
		static constexpr uint32_t s_delayBackRightID  = Utilities::GetBoardChannelUUID(8, 11);
		static constexpr uint32_t s_anodeFrontID      = Utilities::GetBoardChannelUUID(8, 13);
		static constexpr uint32_t s_anodeBackID       = Utilities::GetBoardChannelUUID(8, 15);
	};

}