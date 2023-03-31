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
		m_openFlag(false)
	{
	}

	SourceDialog::~SourceDialog()
	{
	}

	bool SourceDialog::ImGuiRenderSourceDialog()
	{
		SPEC_PROFILE_FUNCTION();
		static bool result = false;
		static std::vector<DataSource::SourceType> availTypes = { DataSource::SourceType::CompassOnline, DataSource::SourceType::CompassOffline, DataSource::SourceType::DaqromancyOnline,
																  DataSource::SourceType::DaqromancyOffline, DataSource::SourceType::CharonOnline, DataSource::SourceType::RitualOnline };
		result = false;
		if (m_openFlag)
		{
			m_openFlag = false;
			m_args.type = DataSource::SourceType::None;
			m_args.location = "";
			m_args.port = "52324";
			m_args.coincidenceWindow = 3000000;
			m_args.bitflags = 0;
			ImGui::OpenPopup(ICON_FA_LINK " Attach Source");
		}
		if (ImGui::BeginPopupModal(ICON_FA_LINK " Attach Source"))
		{
			if (ImGui::BeginCombo("Source Type", ConvertDataSourceTypeToString(m_args.type).c_str()))
			{
				for (auto& type : availTypes)
				{
					if (ImGui::Selectable(ConvertDataSourceTypeToString(type).c_str(), type == m_args.type, ImGuiSelectableFlags_DontClosePopups))
					{
						m_args.type = type;
					}
				}
				ImGui::EndCombo();
			}

			if (m_args.type == DataSource::SourceType::CompassOnline)
			{
				ImGui::InputText("Hostname", &m_args.location);
				ImGui::InputText("Port", &m_args.port);
				if (ImGui::RadioButton("Energy", (m_args.bitflags & CompassHeaders::Energy) != 0))
				{
					m_args.bitflags = m_args.bitflags ^ CompassHeaders::Energy;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("Energy Short", (m_args.bitflags & CompassHeaders::EnergyShort) != 0))
				{
					m_args.bitflags = m_args.bitflags ^ CompassHeaders::EnergyShort;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("Energy Calibrated", (m_args.bitflags & CompassHeaders::EnergyCalibrated) != 0))
				{
					m_args.bitflags = m_args.bitflags ^ CompassHeaders::EnergyCalibrated;
				}
				ImGui::InputScalar("Coinc. Window (ps)", ImGuiDataType_U64, &m_args.coincidenceWindow);
			}
			else if (m_args.type == DataSource::SourceType::CompassOffline)
			{
				ImGui::InputText("Run Directory", &m_args.location);
				ImGui::SameLine();
				if (ImGui::Button("Choose Location"))
				{
					m_fileDialog.OpenDialog(FileDialog::Type::OpenDir);
				}
				auto temp = m_fileDialog.RenderFileDialog();
				if (!temp.first.empty() && temp.second == FileDialog::Type::OpenDir)
					m_args.location = temp.first;
				ImGui::InputScalar("Coinc. Window (ps)", ImGuiDataType_U64, &m_args.coincidenceWindow);
			}
			else if (m_args.type == DataSource::SourceType::DaqromancyOnline)
			{
				ImGui::InputText("Hostname", &m_args.location);
				ImGui::InputText("Port", &m_args.port);
				ImGui::InputScalar("Coinc. Window (ps)", ImGuiDataType_U64, &m_args.coincidenceWindow);
			}
			else if (m_args.type == DataSource::SourceType::DaqromancyOffline)
			{
				ImGui::InputText("Run Directory", &m_args.location);
				ImGui::SameLine();
				if (ImGui::Button("Choose Location"))
				{
					m_fileDialog.OpenDialog(FileDialog::Type::OpenDir);
				}
				auto temp = m_fileDialog.RenderFileDialog();
				if (!temp.first.empty() && temp.second == FileDialog::Type::OpenDir)
					m_args.location = temp.first;
				ImGui::InputScalar("Coinc. Window (ps)", ImGuiDataType_U64, &m_args.coincidenceWindow);
			}
			else if (m_args.type == DataSource::SourceType::CharonOnline)
			{
				ImGui::InputText("Hostname", &m_args.location);
				ImGui::InputText("Port", &m_args.port);
			}
			else if (m_args.type == DataSource::SourceType::RitualOnline)
			{
				ImGui::InputText("Hostname", &m_args.location);
				ImGui::InputText("Port", &m_args.port);
				ImGui::InputScalar("Coinc. Window (ps)", ImGuiDataType_U64, &m_args.coincidenceWindow);
			}

			if (ImGui::Button("Ok"))
			{
				result = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				result = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		return result;
	}

}
