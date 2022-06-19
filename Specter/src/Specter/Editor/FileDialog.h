/*
	FileDialog.h
	File dialog window in ImGui using std::filesystem. This is slightly complicated, as file dialogs change function
	based on the type of action one wants to perform. In our case we have OpenFile, SaveFile, and OpenDirectory. One can also
	specify the kind of file (extension). Use FontAwesome icons.

	Use style: 
	if(ImGui::Button())
		Set...FileDialog(true);
	std::string value = ImGuiRender...(extension);

	GWM -- Feb 2022
*/
#ifndef FILE_DIALOG_H
#define FILE_DIALOG_H

#include <filesystem>
#include "imgui.h"

namespace Specter {
	
	class FileDialog
	{
	public:

		enum class Type
		{
			OpenFile,
			SaveFile,
			OpenDir,
			None
		};

		FileDialog();
		~FileDialog();

		inline void OpenDialog(Type type) { m_type = type; m_openDialogFlag = true; }
		std::pair<std::string, Type> RenderFileDialog(const std::string& ext = "");

	private:
		std::string ImGuiRenderOpenFile(const std::string& ext);
		std::string ImGuiRenderSaveFile(const std::string& ext);
		std::string ImGuiRenderOpenDir();

		std::filesystem::path m_currentPath;
		Type m_type;

		std::string m_selectedItem;

		bool m_openDialogFlag;

		ImGuiTableFlags table_flags;
		ImGuiSelectableFlags select_flags;
	};

}

#endif
