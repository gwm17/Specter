/*
	SpectrumDialog.h
	Handles creation of new spectra. Pretty much that simple.

	GWM -- Feb 2022
*/
#ifndef SPECTRUM_DIALOG_H
#define SPECTRUM_DIALOG_H

#include "Specter/Core/Histogram.h"
#include "Specter/Core/Cut.h"

#include "imgui.h"


namespace Specter {

	class SpectrumDialog
	{
	public:
		SpectrumDialog();
		~SpectrumDialog();

		bool ImGuiRenderSpectrumDialog(const std::vector<HistogramArgs>& histoList, const std::vector<CutArgs>& cutList, const std::vector<std::string>& paramList);

		inline void SetSpectrumDialog() { m_openFlag = true; }
	private:
		void RenderDialog1D(const std::vector<std::string>& paramList);
		void RenderDialog2D(const std::vector<std::string>& paramList);
		void RenderDialogSummary(const std::vector<std::string>& paramList);
		void RenderCutDialog(const std::vector<CutArgs>& cutList);

		bool m_openFlag;
		bool m_openCutFlag;
		HistogramArgs m_newParams;
		HistogramArgs m_blank;
		std::vector<std::string> m_subhistos;

		ImGuiSelectableFlags selectFlags;
		ImGuiTableFlags tableFlags;
	};

}

#endif