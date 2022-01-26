#include "SourceDialog.h"
#include "Navigator/Events/PhysicsEvent.h"
#include "Navigator/Events/Event.h"
#include "Navigator/Application.h"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

namespace Navigator {

	SourceDialog::SourceDialog() :
		m_openFlag(false), m_chosenWindow(2000000)
	{
	}

	SourceDialog::~SourceDialog()
	{
	}

	void SourceDialog::ImGuiRenderSourceDialog()
	{
		static bool onlineFlag = false;
		static bool offlineFlag = false;
		static std::vector<DataSource::SourceType> availTypes = { DataSource::SourceType::CompassOnline, DataSource::SourceType::CompassOffline };
		
		if (m_openFlag)
		{
			onlineFlag = false;
			offlineFlag = false;
			m_openFlag = false;
			m_chosenType = DataSource::SourceType::None;
			m_chosenLocation = "";
			m_chosenWindow = 2000000;
			ImGui::OpenPopup("Attach Source");
		}
		if (ImGui::BeginPopupModal("Attach Source"))
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
			}
			else if (m_chosenType == DataSource::SourceType::CompassOffline)
			{
				ImGui::InputText("Run Directory", &m_chosenLocation);
				if (ImGui::Button("Choose Location"))
				{
					m_fileDialog.SetOpenDirDialog(true);
				}
				auto temp = m_fileDialog.ImGuiRenderOpenDir();
				if (temp != "")
					m_chosenLocation = temp;
			}
			ImGui::InputInt("Coincidence Window (ps)", &m_chosenWindow);


			if (ImGui::Button("Ok"))
			{
				PhysicsStartEvent event(m_chosenLocation, m_chosenType, m_chosenWindow);
				Application::Get().OnEvent(event);
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

}
