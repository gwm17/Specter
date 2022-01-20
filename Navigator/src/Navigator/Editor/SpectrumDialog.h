#ifndef SPECTRUM_DIALOG_H
#define SPECTRUM_DIALOG_H

#include "Navigator/HistogramMap.h"
#include "Navigator/ParameterMap.h"
#include "Navigator/CutMap.h"

#include "imgui.h"


namespace Navigator {

	class NAV_API SpectrumDialog
	{
	public:
		SpectrumDialog();
		~SpectrumDialog();

		void ImGuiRenderSpectrumDialog();

		inline void SetSpectrumDialog() { m_openFlag = true; }
	private:
		bool m_openFlag;
		HistogramParameters m_newParams;
		HistogramParameters m_blank;

		ImGuiSelectableFlags selectFlags;
	};

}

#endif