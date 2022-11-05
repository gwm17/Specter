/*
	SourceDialog.cpp
	Handles selection of data source type and location specification. Needs to be updated when new source
	types are added to Specter.

	GWM -- Feb 2022
*/
#include "SourceDialog.h"
#include "Specter/Events/PhysicsEvent.h"
#include "Specter/Events/Event.h"
#include "Specter/Core/Application.h"
#include "Specter/Physics/Caen/CompassHit.h"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "IconsFontAwesome5.h"

namespace Specter {

	SourceDialog::SourceDialog() :
		m_openFlag(false), m_chosenPort("51489"), m_chosenWindow(2000000)
	{
	}

	SourceDialog::~SourceDialog()
	{
	}

	void SourceDialog::ImGuiRenderSourceDialog()
	{
		SPEC_PROFILE_FUNCTION();
		static bool onlineFlag = false;
		static bool offlineFlag = false;
		static std::vector<DataSource::SourceType> availTypes = { DataSource::SourceType::CompassOnline, DataSource::SourceType::CompassOffline, DataSource::SourceType::DaqromancyOnline,
																  DataSource::SourceType::DaqromancyOffline, DataSource::SourceType::CharonOnline };
		
		if (m_openFlag)
		{
			onlineFlag = false;
			offlineFlag = false;
			m_openFlag = false;
			m_chosenType = DataSource::SourceType::None;
			m_chosenLocation = "";
			m_chosenPort = "51489";
			m_chosenWindow = 3000000;
			m_bitflags = 0;
			ImGui::OpenPopup(ICON_FA_LINK " Attach Source");
		}
		if (ImGui::BeginPopupModal(ICON_FA_LINK " Attach Source"))
		{
			if (ImGui::BeginCombo("Source Type", ConvertDataSourceTypeToString(m_chosenType).c_str()))
			{
				for (auto& type : availTypes)
				{
					if (ImGui::Selectable(ConvertDataSourceTypeToString(type).c_str(), type == m_chosenType, ImGuiSelectableFlags_DontClosePopups))
					{
						m_chosenType = type;
					}
				}
				ImGui::EndCombo();
			}

			if (m_chosenType == DataSource::SourceType::CompassOnline)
			{
				ImGui::InputText("Hostname", &m_chosenLocation);
				ImGui::InputText("Port", &m_chosenPort);
				if (ImGui::RadioButton("Energy", (m_bitflags & CompassHeaders::Energy) != 0))
				{
					m_bitflags = m_bitflags ^ CompassHeaders::Energy;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("Energy Short", (m_bitflags & CompassHeaders::EnergyShort) != 0))
				{
					m_bitflags = m_bitflags ^ CompassHeaders::EnergyShort;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("Energy Calibrated", (m_bitflags & CompassHeaders::EnergyCalibrated) != 0))
				{
					m_bitflags = m_bitflags ^ CompassHeaders::EnergyCalibrated;
				}
				ImGui::InputInt("Coinc. Window (ps)", &m_chosenWindow);
			}
			else if (m_chosenType == DataSource::SourceType::CompassOffline)
			{
				ImGui::InputText("Run Directory", &m_chosenLocation);
				ImGui::SameLine();
				if (ImGui::Button("Choose Location"))
				{
					m_fileDialog.OpenDialog(FileDialog::Type::OpenDir);
				}
				auto temp = m_fileDialog.RenderFileDialog();
				if (!temp.first.empty() && temp.second == FileDialog::Type::OpenDir)
					m_chosenLocation = temp.first;
				ImGui::InputInt("Coinc. Window (ps)", &m_chosenWindow);
			}
			else if (m_chosenType == DataSource::SourceType::DaqromancyOnline)
			{
				ImGui::InputText("Hostname", &m_chosenLocation);
				ImGui::InputText("Port", &m_chosenPort);
				ImGui::InputInt("Coinc. Window (ps)", &m_chosenWindow);
			}
			else if (m_chosenType == DataSource::SourceType::DaqromancyOffline)
			{
				ImGui::InputText("Run Directory", &m_chosenLocation);
				ImGui::SameLine();
				if (ImGui::Button("Choose Location"))
				{
					m_fileDialog.OpenDialog(FileDialog::Type::OpenDir);
				}
				auto temp = m_fileDialog.RenderFileDialog();
				if (!temp.first.empty() && temp.second == FileDialog::Type::OpenDir)
					m_chosenLocation = temp.first;
				ImGui::InputInt("Coinc. Window (ps)", &m_chosenWindow);
			}
			else if (m_chosenType == DataSource::SourceType::CharonOnline)
			{
				ImGui::InputText("Hostname", &m_chosenLocation);
				ImGui::InputText("Port", &m_chosenPort);
			}

			if (ImGui::Button("Ok"))
			{
				SPEC_INFO("Here");
				PhysicsStartEvent event(m_chosenLocation, m_chosenType, m_chosenWindow, m_chosenPort, m_bitflags);
				SPEC_INFO("Here");
				Application::Get().OnEvent(event);
				SPEC_INFO("Here");

				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

}
