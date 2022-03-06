/*
	SpectrumDialog.h
	Handles creation of new spectra. Pretty much that simple.

	GWM -- Feb 2022
*/
#include "SpectrumDialog.h"
#include "Navigator/SpectrumManager.h"

#include "misc/cpp/imgui_stdlib.h"

#include "IconsFontAwesome5.h"

namespace Navigator {

	SpectrumDialog::SpectrumDialog()
	{
		selectFlags = ImGuiSelectableFlags_DontClosePopups;
	}

	SpectrumDialog::~SpectrumDialog()
	{
	}

	bool SpectrumDialog::ImGuiRenderSpectrumDialog(const std::vector<HistogramParameters>& histoList, const std::vector<CutParams>& cutList, const std::vector<std::string>& paramList)
	{
		static int dims = 1;
		static std::string selectedCut = "";
		bool result = false;
		if (m_openFlag)
		{
			m_newParams = m_blank;
			m_openFlag = false;
			dims = 1;
			ImGui::OpenPopup(ICON_FA_CHART_BAR " New Spectrum Dialog");
		}

		if (ImGui::BeginPopupModal(ICON_FA_CHART_BAR " New Spectrum Dialog"))
		{
			ImGui::InputText("Spectrum Name", &m_newParams.name);
			ImGui::SliderInt("Dimensions", &dims, 1, 2);
			if (ImGui::BeginTable("SpecParamsTable", 4))
			{
				
				ImGui::TableNextRow();

				ImGui::TableNextColumn();
				if (ImGui::BeginCombo("X Param.", m_newParams.x_par.c_str()))
				{
					for (auto& params : paramList)
					{
						if (ImGui::Selectable(params.c_str(), params == m_newParams.x_par, selectFlags))
							m_newParams.x_par = params;
					}
					ImGui::EndCombo();
				}
				ImGui::TableNextColumn();
				ImGui::InputInt("X Bins", &m_newParams.nbins_x);
				ImGui::TableNextColumn();
				ImGui::InputDouble("Min X", &m_newParams.min_x);
				ImGui::TableNextColumn();
				ImGui::InputDouble("Max X", &m_newParams.max_x);

				if (dims == 2)
				{
					ImGui::TableNextRow();

					ImGui::TableNextColumn();
					if (ImGui::BeginCombo("Y Param.", m_newParams.y_par.c_str()))
					{
						for (auto& params : paramList)
						{
							if (ImGui::Selectable(params.c_str(), params == m_newParams.y_par, selectFlags))
								m_newParams.y_par = params;
						}
						ImGui::EndCombo();
					}
					ImGui::TableNextColumn();
					ImGui::InputInt("Y Bins", &m_newParams.nbins_y);
					ImGui::TableNextColumn();
					ImGui::InputDouble("Min Y", &m_newParams.min_y);
					ImGui::TableNextColumn();
					ImGui::InputDouble("Max Y", &m_newParams.max_y);
				}

				ImGui::EndTable();
			}
			if (ImGui::TreeNode("Applied Cuts"))
			{
				for (auto& name : m_newParams.cutsAppliedTo)
				{
					ImGui::BulletText("%s", name.c_str());
				}
				ImGui::TreePop();
			}

			if (ImGui::Button("Apply Cuts"))
			{
				selectedCut = "";
				ImGui::OpenPopup("Cut List");
			}
			if (ImGui::BeginPopup("Cut List"))
			{
				for (auto& cut : cutList)
				{
					if (ImGui::Selectable(cut.name.c_str(), cut.name == selectedCut, selectFlags))
						selectedCut = cut.name;
				}
				ImGui::InputText("Selected Cut", &selectedCut);
				if (ImGui::Button("Ok"))
				{
					m_newParams.cutsAppliedTo.push_back(selectedCut);
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			if (ImGui::Button("Ok"))
			{
				SpectrumManager::GetInstance().AddHistogram(m_newParams);
				ImGui::CloseCurrentPopup();
				result = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
				result = false;
			}
			ImGui::EndPopup();
		}
		return result;
	}
}