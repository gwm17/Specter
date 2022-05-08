/*
	SpectrumDialog.h
	Handles creation of new spectra. Pretty much that simple.

	GWM -- Feb 2022
*/
#include "SpectrumDialog.h"
#include "Navigator/Core/SpectrumManager.h"

#include "misc/cpp/imgui_stdlib.h"

#include "IconsFontAwesome5.h"

namespace Navigator {

	SpectrumDialog::SpectrumDialog() :
		m_openFlag(false), m_openCutFlag(false)
	{
		selectFlags = ImGuiSelectableFlags_DontClosePopups;
		tableFlags = ImGuiTableFlags_BordersH | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_RowBg;
	}

	SpectrumDialog::~SpectrumDialog()
	{
	}

	bool SpectrumDialog::ImGuiRenderSpectrumDialog(const std::vector<HistogramArgs>& histoList, const std::vector<CutArgs>& cutList, const std::vector<std::string>& paramList)
	{
		NAV_PROFILE_FUNCTION();
		static std::string selectedCut = "";
		bool result = false;
		if (m_openFlag)
		{
			m_openFlag = false;
			m_newParams = m_blank;
			m_subhistos.clear();
			ImGui::OpenPopup(ICON_FA_CHART_BAR " New Spectrum Dialog");
		}

		if (ImGui::BeginPopupModal(ICON_FA_CHART_BAR " New Spectrum Dialog"))
		{
			ImGui::InputText("Spectrum Name", &m_newParams.name);
			if (ImGui::BeginCombo("Spectrum Type", ConvertSpectrumTypeToString(m_newParams.type).c_str()))
			{
				if (ImGui::Selectable("Histogram1D", m_newParams.type == SpectrumType::Histo1D, selectFlags))
					m_newParams.type = SpectrumType::Histo1D;
				else if (ImGui::Selectable("Histogram2D", m_newParams.type == SpectrumType::Histo2D, selectFlags))
					m_newParams.type = SpectrumType::Histo2D;
				else if (ImGui::Selectable("Summary", m_newParams.type == SpectrumType::Summary, selectFlags))
					m_newParams.type = SpectrumType::Summary;
				ImGui::EndCombo();
			}
			
			switch (m_newParams.type)
			{
			case SpectrumType::Histo1D: RenderDialog1D(paramList); break;
			case SpectrumType::Histo2D: RenderDialog2D(paramList); break;
			case SpectrumType::Summary: RenderDialogSummary(paramList); break;
			case SpectrumType::None: break;
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
				m_openCutFlag = true;
			}
			RenderCutDialog(cutList);

			if (ImGui::Button("Ok"))
			{
				switch (m_newParams.type)
				{
				case SpectrumType::Histo1D: SpectrumManager::GetInstance().AddHistogram(m_newParams); break;
				case SpectrumType::Histo2D: SpectrumManager::GetInstance().AddHistogram(m_newParams); break;
				case SpectrumType::Summary: SpectrumManager::GetInstance().AddHistogramSummary(m_newParams, m_subhistos); break;
				case SpectrumType::None: break;
				}
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

	void SpectrumDialog::RenderDialog1D(const std::vector<std::string>& paramList)
	{
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

			ImGui::EndTable();
		}
	}

	void SpectrumDialog::RenderDialog2D(const std::vector<std::string>& paramList)
	{
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
			

			ImGui::EndTable();
		}
	}

	void SpectrumDialog::RenderDialogSummary(const std::vector<std::string>& paramList)
	{
		if (ImGui::BeginTable("SpecParamsTable", 3))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::InputInt("X Bins", &m_newParams.nbins_x);
			ImGui::TableNextColumn();
			ImGui::InputDouble("Min X", &m_newParams.min_x);
			ImGui::TableNextColumn();
			ImGui::InputDouble("Max X", &m_newParams.max_x);
			ImGui::EndTable();
		}
		if (ImGui::TreeNode("Selected Parameters"))
		{
			for (auto& name : m_subhistos)
			{
				ImGui::BulletText("%s", name.c_str());
			}
			ImGui::TreePop();
		}
		if (ImGui::Button("Add Parameter"))
		{
			m_subhistos.clear();
			ImGui::OpenPopup("Param List");
		}
		if (ImGui::BeginPopupModal("Param List"))
		{
			if (ImGui::Button("Ok"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				m_subhistos.clear();
				ImGui::CloseCurrentPopup();
			}

			auto check_func = [this](const std::string& name)
			{
				for (auto& par : m_subhistos)
				{
					if (name == par)
						return true;
				}
				return false;
			};

			if (ImGui::BeginTable("Parameters", 1, tableFlags, ImVec2(-1, -1)))
			{
				for (auto& param : paramList)
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					if (ImGui::Selectable(param.c_str(), check_func(param), selectFlags))
					{
						if (!check_func(param))
						{
							m_subhistos.push_back(param);
						}
						else
						{
							auto iter = std::remove(m_subhistos.begin(), m_subhistos.end(), param);
							m_subhistos.erase(iter, m_subhistos.end());
						}
					}
				}

				ImGui::EndTable();
			}
			
			ImGui::EndPopup();
		}
	}

	void SpectrumDialog::RenderCutDialog(const std::vector<CutArgs>& cutList)
	{
		static std::string selectedCut = "";
		if (m_openCutFlag)
		{
			selectedCut = "";
			m_openCutFlag = false;
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
	}
}