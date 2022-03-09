#include "SPSInputLayer.h"
#include "imgui.h"

namespace Navigator {

	SPSInputLayer::SPSInputLayer() :
		Layer("SPSInputLayer"), x1_weight("x1_weight"), x2_weight("x2_weight"), m_bfield(0.0), m_theta(0.0), m_beamKE(0.0),
		m_targMass(0.0), m_projMass(0.0), m_ejectMass(0.0), m_residMass(0.0)
	{
		for (int i = 0; i < 2; i++)
		{
			m_targNums[i] = 0;
			m_projNums[i] = 0;
			m_ejectNums[i] = 0;
			m_residNums[i] = 0;
		}

		SpectrumManager& manager = SpectrumManager::GetInstance();
		manager.BindVariable(x1_weight);
		manager.BindVariable(x2_weight);
	}

	SPSInputLayer::~SPSInputLayer() {}

	void SPSInputLayer::OnAttach() {}

	void SPSInputLayer::OnDetach() {}

	void SPSInputLayer::OnUpdate() {}

	void SPSInputLayer::OnEvent(Event& event) {}

	void SPSInputLayer::OnImGuiRender()
	{
		ImGui::SetCurrentContext(ImGui::GetCurrentContext());
		if (ImGui::Begin("SPS Input"))
		{
			ImGui::InputDouble("Bfield(kG)", &m_bfield, 0.01, 0.1);
			ImGui::InputDouble("Theta(deg)", &m_theta, 0.1, 1.0);
			ImGui::InputDouble("BeamKE(MeV)", &m_beamKE, 0.1, 1.0);
			ImGui::InputInt2("Target Z,A", m_targNums);
			ImGui::InputInt2("Projectile Z,A", m_projNums);
			ImGui::InputInt2("Ejectile Z,A", m_ejectNums);
			if (ImGui::Button("Set"))
			{
				UpdateWeights();
			}
			ImGui::End();
		}
	}

	void SPSInputLayer::UpdateWeights()
	{
		for (int i = 0; i < 2; i++)
			m_residNums[i] = m_targNums[i] + m_projNums[i] - m_ejectNums[i];
		if (m_residNums[0] < 0 || m_residNums[1] <= 0)
		{
			NAV_ERROR("Invalid residual nucleus at SPSInputLayer::UpdateMasses()! ZR: {0} AR: {1}", m_residNums[0], m_residNums[1]);
			return;
		}

		m_targMass = m_masses.FindMass(m_targNums[0], m_targNums[1]);
		m_projMass = m_masses.FindMass(m_projNums[0], m_projNums[1]);
		m_ejectMass = m_masses.FindMass(m_ejectNums[0], m_ejectNums[1]);
		m_residMass = m_masses.FindMass(m_residNums[0], m_residNums[1]);
		if (m_targMass == 0.0 || m_projMass == 0.0 || m_ejectMass == 0.0 || m_residMass == 0.0)
			return;

	}
}