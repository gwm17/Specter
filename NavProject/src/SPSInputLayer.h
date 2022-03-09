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
		virtual void OnUpdate() override;
		virtual void OnEvent(Event& event) override;
		virtual void OnImGuiRender() override;

	private:
		void UpdateWeights();

		//Variables for use in analysis
		NavVariable x1_weight;
		NavVariable x2_weight;

		//UI facing inputs
		double m_bfield; //kG
		double m_theta; //deg
		double m_beamKE; //MeV
		int m_targNums[2];
		int m_projNums[2];
		int m_ejectNums[2];
		int m_residNums[2];

		//Internal values
		double m_targMass, m_projMass, m_ejectMass, m_residMass;

		//Map for mass table
		MassMap m_masses;
	};

}

#endif