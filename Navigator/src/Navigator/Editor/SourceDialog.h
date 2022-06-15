/*
	SourceDialog.h
	Handles selection of data source type and location specification. Needs to be updated when new source
	types are added to Navigator.

	GWM -- Feb 2022
*/
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
		uint16_t m_bitflags;
		int m_chosenWindow;
		int m_channels_per_board;
	};

}

#endif