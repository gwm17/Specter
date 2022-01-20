#include "SpectrumDialog.h"

#include "misc/cpp/imgui_stdlib.h"

namespace Navigator {

	SpectrumDialog::SpectrumDialog()
	{
		selectFlags = ImGuiSelectableFlags_DontClosePopups;
	}

	SpectrumDialog::~SpectrumDialog()
	{
	}

	void SpectrumDialog::ImGuiRenderSpectrumDialog()
	{
		static int dims = 1;
		static std::string selectedCut = "";
		if (m_openFlag)
		{
			m_newParams = m_blank;
			m_openFlag = false;
			dims = 1;
			ImGui::OpenPopup("New Spectrum Dialog");
		}

		if (ImGui::BeginPopupModal("New Spectrum Dialog"))
		{
			ParameterMap& parMap = ParameterMap::GetInstance();
			ImGui::InputText("Spectrum Name", &m_newParams.name);
			ImGui::SliderInt("Dimensions", &dims, 1, 2);
			if (ImGui::BeginTable("SpecParamsTable", 4))
			{
				
				ImGui::TableNextRow();

				ImGui::TableNextColumn();
				if (ImGui::BeginCombo("X Parameter", m_newParams.x_par.c_str()))
				{
					for (auto& params : parMap)
					{
						if (ImGui::Selectable(params.first.c_str(), params.first == m_newParams.x_par, selectFlags))
							m_newParams.x_par = params.first;
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
					if (ImGui::BeginCombo("Y Parameter", m_newParams.y_par.c_str()))
					{
						for (auto& params : parMap)
						{
							if (ImGui::Selectable(params.first.c_str(), params.first == m_newParams.y_par, selectFlags))
								m_newParams.y_par = params.first;
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
					ImGui::BulletText(name.c_str());
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
				CutMap& cutMap = CutMap::GetInstance();
				for (auto& cut : cutMap)
				{
					if (ImGui::Selectable(cut.first.c_str(), cut.first == selectedCut, selectFlags))
						selectedCut = cut.first;
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
				HistogramMap::GetInstance().AddHistogram(m_newParams);
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