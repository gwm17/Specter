/*
	FileDialog.cpp
	File dialog window in ImGui using std::filesystem. This is slightly complicated, as file dialogs change function
	based on the type of action one wants to perform. In our case we have OpenFile, SaveFile, and OpenDirectory. One can also
	specify the kind of file (extension). Use FontAwesome icons.

	Use style:
	if(ImGui::Button())
		Set...FileDialog(true);
	std::string value = ImGuiRender...(extension);

	GWM -- Feb 2022
*/
#include "FileDialog.h"
#include "misc/cpp/imgui_stdlib.h"
#include "IconsFontAwesome5.h"

namespace Specter {

	//Helper function to handle file size printing
	std::string ConvertFileSystemSizeToString(std::uintmax_t value)
	{
		SPEC_PROFILE_FUNCTION();
		int i = 0;
		double mantissa = (double)value;
		for (; mantissa >= 1024.0; ++i)
			mantissa /= 1024.0;
		mantissa = std::ceil(mantissa * 10.0) / 10.0;
		return std::to_string(int(mantissa)) + "BKMGTPE"[i];
	}

	FileDialog::FileDialog() :
		m_currentPath(std::filesystem::current_path()), m_type(Type::None), m_selectedItem(""), m_openDialogFlag(false)
	{
		table_flags = ImGuiTableFlags_BordersH | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_RowBg;
		select_flags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_DontClosePopups;
	}
	
	FileDialog::~FileDialog() {}

	//Each type of action has its own render function

	std::pair<std::string, FileDialog::Type> FileDialog::RenderFileDialog(const std::string& ext)
	{
		SPEC_PROFILE_FUNCTION();
		if (m_openDialogFlag)
		{
			m_selectedItem = "";
			m_openDialogFlag = false;
			m_currentPath = std::filesystem::current_path();
			ImGui::OpenPopup("File Dialog");
		}

		std::string result = "";
		if (ImGui::BeginPopupModal("File Dialog"))
		{
			switch (m_type)
			{
				case Type::OpenFile:
				{
					result = ImGuiRenderOpenFile(ext);
					break;
				}
				case Type::SaveFile: 
				{
					result = ImGuiRenderSaveFile(ext);
					break;
				}
				case Type::OpenDir:
				{
					result = ImGuiRenderOpenDir();
					break;
				}
				case Type::None: break;
			}
			ImGui::EndPopup();
		}

		return std::make_pair(result, m_type);
	}

	std::string FileDialog::ImGuiRenderOpenFile(const std::string& ext)
	{
		SPEC_PROFILE_FUNCTION();
		std::string result = "";
		std::string text = "";
		
		ImGui::Text("%s", ("Current Directory: " + m_currentPath.lexically_normal().string()).c_str());
		ImGui::SameLine();
		ImGui::Text("%s", ("Extension Filter: "+ext).c_str());
		ImGui::InputText("Selected", &m_selectedItem);
		if (ImGui::Button("Ok"))
		{
			std::filesystem::path filepath = m_currentPath / m_selectedItem;
			result = filepath.string();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
			ImGui::CloseCurrentPopup();

		if (ImGui::BeginTable("File System", 2, table_flags, ImVec2(-1, -1)))
		{
			ImGui::TableSetupColumn("Name");
			ImGui::TableSetupColumn("Size");
			ImGui::TableHeadersRow();
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			if (ImGui::Selectable(ICON_FA_FOLDER " ..", false, select_flags))
			{
				m_selectedItem.clear();
				m_currentPath.append("..");
			}
			ImGui::TableNextColumn();
			ImGui::Text("N/A");
			for (auto& entry : std::filesystem::directory_iterator(m_currentPath))
			{
				if (entry.is_directory())
				{
					ImGui::TableNextRow();
					text = ICON_FA_FOLDER " " + std::filesystem::relative(entry.path(), m_currentPath).string();
					ImGui::TableNextColumn();
					if (ImGui::Selectable(text.c_str(), false, select_flags))
					{
						m_selectedItem.clear();
						m_currentPath /= entry.path();
					}
					ImGui::TableNextColumn();
					ImGui::Text("N/A");
				}
				else if (entry.path().filename().extension() == ext)
				{
					ImGui::TableNextRow();
					text = ICON_FA_FILE " " + entry.path().filename().string();
					ImGui::TableNextColumn();
					if (ImGui::Selectable(text.c_str(), false, select_flags))
						m_selectedItem = entry.path().filename().string();
					ImGui::TableNextColumn();
					ImGui::Text("%s", ConvertFileSystemSizeToString(entry.file_size()).c_str());
				}
			}
			ImGui::EndTable();
		}
		return result;
	}

	std::string FileDialog::ImGuiRenderSaveFile(const std::string& ext)
	{
		SPEC_PROFILE_FUNCTION();
		std::string result = "";
		std::string text = "";

		ImGui::Text("%s", ("Current Directory: "+m_currentPath.lexically_normal().string()).c_str());
		ImGui::SameLine();
		ImGui::Text("%s", ("Extension Filter: "+ext).c_str());
		ImGui::InputText("Selected", &m_selectedItem);
		if (ImGui::Button("Ok"))
		{
			std::filesystem::path filepath = m_currentPath / m_selectedItem;
			result = filepath.string();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
			ImGui::CloseCurrentPopup();

		if (ImGui::BeginTable("File System", 2, table_flags, ImVec2(-1, -1)))
		{
			ImGui::TableSetupColumn("Name");
			ImGui::TableSetupColumn("Size");
			ImGui::TableHeadersRow();
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			if (ImGui::Selectable(ICON_FA_FOLDER " ..", false, select_flags))
			{
				m_selectedItem.clear();
				m_currentPath.append("..");
			}
			ImGui::TableNextColumn();
			ImGui::Text("N/A");
			for (auto& entry : std::filesystem::directory_iterator(m_currentPath))
			{
				if (entry.is_directory())
				{
					ImGui::TableNextRow();
					text = ICON_FA_FOLDER " " + std::filesystem::relative(entry.path(), m_currentPath).string();
					ImGui::TableNextColumn();
					if (ImGui::Selectable(text.c_str(), false, select_flags))
					{
						m_selectedItem.clear();
						m_currentPath /= entry.path();
					}
					ImGui::TableNextColumn();
					ImGui::Text("N/A");
				}
				else if (entry.path().filename().extension() == ext)
				{
					ImGui::TableNextRow();
					text = ICON_FA_FILE " " + entry.path().filename().string();
					ImGui::TableNextColumn();
					if (ImGui::Selectable(text.c_str(), false, select_flags))
						m_selectedItem = entry.path().filename().string();
					ImGui::TableNextColumn();
					ImGui::Text("%s", ConvertFileSystemSizeToString(entry.file_size()).c_str());
				}
			}
			ImGui::EndTable();
		}
		return result;
	}

	std::string FileDialog::ImGuiRenderOpenDir()
	{
		SPEC_PROFILE_FUNCTION();
		std::string result = "";
		std::string text = "";
		
		ImGui::Text("%s", ("Current Directory: "+m_currentPath.lexically_normal().string()).c_str());
		ImGui::InputText("Selected", &m_selectedItem);
		if (ImGui::Button("Ok"))
		{
			result = m_selectedItem;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
			ImGui::CloseCurrentPopup();

		if (ImGui::BeginTable("File System", 2, table_flags, ImVec2(-1, -1)))
		{
			ImGui::TableSetupColumn("Name");
			ImGui::TableSetupColumn("Size");
			ImGui::TableHeadersRow();
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			if (ImGui::Selectable(ICON_FA_FOLDER " ..", false, select_flags))
			{
				m_currentPath.append("..");
				m_selectedItem = m_currentPath.string();
			}
			ImGui::TableNextColumn();
			ImGui::Text("N/A");
			for (auto& entry : std::filesystem::directory_iterator(m_currentPath))
			{
				ImGui::TableNextRow();
				if (entry.is_directory())
				{
					text = ICON_FA_FOLDER " " + std::filesystem::relative(entry.path(), m_currentPath).string();
					ImGui::TableNextColumn();
					if (ImGui::Selectable(text.c_str(), false, select_flags))
					{
						m_currentPath /= entry.path();
						m_selectedItem = m_currentPath.string();
					}
					ImGui::TableNextColumn();
					ImGui::Text("N/A");
				}
				else
				{
					text = ICON_FA_FILE " " + entry.path().filename().string();
					ImGui::TableNextColumn();
					ImGui::Text("%s", text.c_str());
					ImGui::TableNextColumn();
					ImGui::Text("%s", ConvertFileSystemSizeToString(entry.file_size()).c_str());
				}
			}
			ImGui::EndTable();
		}
		return result;
	}

}
