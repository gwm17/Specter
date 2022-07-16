/*
	SPSInputLayer.cpp
	An example of what a user created layer might look like. This is how one would extend the base editor to have more
	functionality, specific to their experiment/setup. In this case, we provide inputs for reaction information so that
	the kinematic shift of the SE-SPS focal plane can be calculated, and weights for tracing particle trajectories are
	produced for use in analysis (as NavVariables).

	A reminder that these layers should not be that intense. The more work that is shoved into the UI, the less responsive
	and more sluggish overall the UI will become. The vast bulk of the analysis work should be left to the PhysicsLayer which has its own
	thread to work upon.

	GWM -- Feb 2022
*/
#include "SPSInputLayer.h"
#include "imgui.h"

namespace Specter {

	SPSInputLayer::SPSInputLayer(const SpectrumManager::Ref& manager) :
		Layer("SPSInputLayer"), x1_weight("x1_weight"), x2_weight("x2_weight"), m_bfield(0.0), m_theta(0.0), m_beamKE(0.0),
		m_rxnEqn("")
	{
		for (int i = 0; i < 2; i++)
		{
			m_targNums[i] = 0;
			m_projNums[i] = 0;
			m_ejectNums[i] = 0;
			m_residNums[i] = 0;
		}

		manager->BindVariable(x1_weight);
		manager->BindVariable(x2_weight);
	}

	SPSInputLayer::~SPSInputLayer() {}

	void SPSInputLayer::OnAttach() {}

	void SPSInputLayer::OnDetach() {}

	void SPSInputLayer::OnUpdate(Timestep& step) {}

	void SPSInputLayer::OnEvent(Event& event) {}

	void SPSInputLayer::OnImGuiRender()
	{
		SPEC_PROFILE_FUNCTION();
		if (ImGui::Begin("SPS Input"))
		{
			//Create widgets for all of our inputs
			ImGui::InputDouble("Bfield(kG)", &m_bfield, 0.01, 0.1);
			ImGui::InputDouble("Theta(deg)", &m_theta, 0.1, 1.0);
			ImGui::InputDouble("BeamKE(MeV)", &m_beamKE, 0.1, 1.0);
			ImGui::InputInt2("Target Z,A", m_targNums);
			ImGui::InputInt2("Projectile Z,A", m_projNums);
			ImGui::InputInt2("Ejectile Z,A", m_ejectNums);
			if (ImGui::Button("Set"))
			{
				//We dont want to calculate the weights every frame, so
				//we lock that calculation behind a button.
				UpdateWeights();
			}
			//Display some info about the internal state
			ImGui::Text("-------Current Settings-------");
			ImGui::Text("Reaction Equation: ");
			ImGui::SameLine();
			ImGui::Text("%s", m_rxnEqn.c_str());
			ImGui::Text("X1 Weight: %f", x1_weight.GetValue());
			ImGui::Text("X2 Weight: %f", x2_weight.GetValue());
		}
		ImGui::End();
	}

	void SPSInputLayer::UpdateWeights()
	{
		SPEC_PROFILE_FUNCTION();
		m_rxnEqn = ""; //reset

		//Calculate residual nucleus from reaction
		for (int i = 0; i < 2; i++)
			m_residNums[i] = m_targNums[i] + m_projNums[i] - m_ejectNums[i];
		if (m_residNums[0] < 0 || m_residNums[1] <= 0)
		{
			SPEC_ERROR("Invalid residual nucleus at SPSInputLayer::UpdateMasses()! ZR: {0} AR: {1}", m_residNums[0], m_residNums[1]);
			return;
		}

		if (m_bfield == 0.0 || m_beamKE == 0.0)
		{
			SPEC_ERROR("Invaild kinematic settings at SPSInputLayer::UpdateWeights()! BeamKE: {0} Bfield: {1}", m_beamKE, m_bfield);
			return;
		}

		//Obtain masses from the AMDC table
		double targMass = m_masses.FindMass(m_targNums[0], m_targNums[1]);
		double projMass = m_masses.FindMass(m_projNums[0], m_projNums[1]);
		double ejectMass = m_masses.FindMass(m_ejectNums[0], m_ejectNums[1]);
		double residMass = m_masses.FindMass(m_residNums[0], m_residNums[1]);
		if (targMass == 0.0 || projMass == 0.0 || ejectMass == 0.0 || residMass == 0.0)
			return;

		std::string temp;
		temp = m_masses.FindSymbol(m_targNums[0], m_targNums[1]);
		m_rxnEqn += temp + "(";
		temp = m_masses.FindSymbol(m_projNums[0], m_projNums[1]);
		m_rxnEqn += temp + ",";
		temp = m_masses.FindSymbol(m_ejectNums[0], m_ejectNums[1]);
		m_rxnEqn += temp + ")";
		temp = m_masses.FindSymbol(m_residNums[0], m_residNums[1]);
		m_rxnEqn += temp;

		double theta_rad = m_theta * c_deg2rad; //convert to radians
		double bfield_t = m_bfield * 0.1; //convert to tesla
		double Q = targMass + projMass - ejectMass - residMass;
		//kinematics a la Iliadis p.590
		double term1 = std::sqrt(projMass * ejectMass * m_beamKE) / (ejectMass + residMass) * std::cos(theta_rad);
		double term2 = (m_beamKE * (residMass - projMass) + residMass * Q) / (ejectMass + residMass);

		double ejectKE = term1 + std::sqrt(term1 * term1 + term2);
		ejectKE *= ejectKE;

		//momentum
		double ejectP = std::sqrt(ejectKE * (ejectKE + 2.0 * ejectMass));

		//calculate rho from B a la B*rho = (proj. momentum)/(proj. charge)
		double rho = (ejectP * c_mev2j) / (m_ejectNums[0] * c_e * c_C * bfield_t) * 100.0; //in cm

		double K;
		K = sqrt(projMass * ejectMass * m_beamKE / ejectKE);
		K *= std::sin(theta_rad);

		double denom = ejectMass + residMass - std::sqrt(projMass * ejectMass * m_beamKE / ejectKE) * std::cos(theta_rad);

		K /= denom;
		double zshift = -1 * rho * c_spsDisp * c_spsMag * K; //delta-Z in cm
		x1_weight.SetValue((0.5 - zshift / c_wireDist));
		x2_weight.SetValue((1.0 - x1_weight.GetValue()));
	}
}
