/*
	SpectrumDialog.h
	Handles creation of new spectra. Pretty much that simple.

	GWM -- Feb 2022
*/
#ifndef SPECTRUM_DIALOG_H
#define SPECTRUM_DIALOG_H

#include "Navigator/Histogram.h"
#include "Navigator/Cut.h"

#include "imgui.h"


namespace Navigator {

	class NAV_API SpectrumDialog
	{
	public:
		SpectrumDialog();
		~SpectrumDialog();

		bool ImGuiRenderSpectrumDialog(const std::vector<HistogramParameters>& histoList, const std::vector<CutParams>& cutList, const std::vector<std::string>& paramList);

		inline void SetSpectrumDialog() { m_openFlag = true; }
	private:
		void RenderDialog1D(const std::vector<std::string>& paramList);
		void RenderDialog2D(const std::vector<std::string>& paramList);
		void RenderDialogSummary(const std::vector<std::string>& paramList);
		void RenderCutDialog(const std::vector<CutParams>& cutList);

		bool m_openFlag;
		bool m_openCutFlag;
		HistogramParameters m_newParams;
		HistogramParameters m_blank;
		std::vector<std::string> m_subhistos;

		ImGuiSelectableFlags selectFlags;
	};

}

#endif