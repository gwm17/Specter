/*
	SPSInputLayer.h
	An example of what a user created layer might look like. This is how one would extend the base editor to have more
	functionality, specific to their experiment/setup. In this case, we provide inputs for reaction information so that
	the kinematic shift of the SE-SPS focal plane can be calculated, and weights for tracing particle trajectories are
	produced for use in analysis (as Variables).

	A reminder that these layers should not be that intense. The more work that is shoved into the UI, the less responsive
	and more sluggish overall the UI will become. The vast bulk of the analysis work should be left to the PhysicsLayer which has its own
	thread to work upon.

	GWM -- Feb 2022
*/
#ifndef SPS_INPUT_LAYER_H
#define SPS_INPUT_LAYER_H

#include "Navigator.h"
#include "MassMap.h"

namespace Navigator {

	class SPSInputLayer : public Layer
	{
	public:
		SPSInputLayer();
		~SPSInputLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep& step) override;
		virtual void OnEvent(Event& event) override; //If you want to respond to events
		virtual void OnImGuiRender() override; //"Main" function

	private:
		void UpdateWeights();

		//Variables for use in analysis
		Variable x1_weight;
		Variable x2_weight;

		//UI facing inputs
		double m_bfield; //kG
		double m_theta; //deg
		double m_beamKE; //MeV
		//Z, A inputs for reaction nuclei
		int m_targNums[2];
		int m_projNums[2];
		int m_ejectNums[2];
		int m_residNums[2];

		//Text for UI
		std::string m_rxnEqn;

		//Map for mass table
		MassMap m_masses;

		static constexpr double c_mev2j = 1.60218E-13; //J per MeV
		static constexpr double c_e = 1.602E-19; //unit charge Coulombs
		static constexpr double c_C = 2.9979E8; //speed of light m/s
		static constexpr double c_spsDisp = 1.96; //dispersion (x/rho)
		static constexpr double c_spsMag = 0.39; //magnification in x
		static constexpr double c_wireDist = 4.28625; //FPD anode wire separation in cm
		static constexpr double c_deg2rad = 3.14159265358979323846 / 180.0; //pi/180 
	};

}

#endif