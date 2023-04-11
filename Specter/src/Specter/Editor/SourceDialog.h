/*
	SourceDialog.h
	Handles selection of data source type and location specification. Needs to be updated when new source
	types are added to Specter.

	GWM -- Feb 2022
*/
#ifndef SOURCE_DIALOG_H
#define SOURCE_DIALOG_H

#include "FileDialog.h"
#include "Specter/Physics/DataSource.h"

namespace Specter {

	class SourceDialog
	{
	public:
		SourceDialog();
		~SourceDialog();

		bool ImGuiRenderSourceDialog();

		const SourceArgs& GetArgs() const { return m_args; }

		void OpenSourceDialog() { m_openFlag = true; }
	private:
		bool m_openFlag;
		SourceArgs m_args;
		FileDialog m_fileDialog;
	};

}

#endif