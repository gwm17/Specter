#ifndef FILE_DIALOG_H
#define FILE_DIALOG_H

#include <filesystem>
#include "imgui.h"

namespace Navigator {

	class NAV_API FileDialog
	{
	public:
		FileDialog();
		~FileDialog();

		std::string ImGuiRenderOpenFile(const std::string& ext);
		std::string ImGuiRenderSaveFile(const std::string& ext);
		std::string ImGuiRenderOpenDir();
		
		inline void ResetCurrentDirectory() { m_currentPath = std::filesystem::current_path();  }

		inline void SetOpenFileDialog(bool value) { m_openFileFlag = value; }
		inline void SetSaveFileDialog(bool value) { m_saveFileFlag = value; }
		inline void SetOpenDirDialog(bool value) { m_openDirFlag = value; }

		inline bool IsOpenFileOpen() { return m_openFileFlag; }
		inline bool IsOpenDirOpen() { return m_openDirFlag; }
		inline bool IsSaveFileOpen() { return m_saveFileFlag;  }
	
		inline const std::string& GetOpenFileWindowName() { return m_openFileName; }
		inline const std::string& GetSaveFileWindowName() { return m_saveFileName; }
		inline const std::string& GetOpenDirWindowName() { return m_openDirName; }
		
	private:
		std::filesystem::path m_currentPath;
		std::string m_openFileName;
		std::string m_saveFileName;
		std::string m_openDirName;

		std::string m_selectedItem;

		bool m_openFileFlag;
		bool m_saveFileFlag;
		bool m_openDirFlag;

		ImGuiTableFlags table_flags;
		ImGuiSelectableFlags select_flags;
	};

}

#endif
