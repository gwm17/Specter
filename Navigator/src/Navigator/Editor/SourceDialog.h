#ifndef SOURCE_DIALOG_H
#define SOURCE_DIALOG_H

#include "FileDialog.h"
#include "Navigator/Physics/DataSource.h"

namespace Navigator {

	class NAV_API SourceDialog
	{
	public:
		SourceDialog();
		~SourceDialog();

		void ImGuiRenderSourceDialog();

		inline void OpenSourceDialog() { m_openFlag = true; }
	private:
		bool m_openFlag;
		DataSource::SourceType m_chosenType;
		std::string m_chosenLocation;
		std::string m_chosenPort;
		FileDialog m_fileDialog;
		int m_chosenWindow;
	};

}

#endif