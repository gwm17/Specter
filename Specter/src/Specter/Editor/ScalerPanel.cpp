#include "ScalerPanel.h"

#include "imgui.h"
#include "implot.h"

namespace Specter {

	ScalerPanel::ScalerPanel() {}

	ScalerPanel::~ScalerPanel() {}

	void ScalerPanel::OnImGuiRender(const SpectrumManager::Ref& manager, const std::vector<std::string>& scalerList, const std::vector<GraphArgs>& graphList)
	{
		if (ImGui::Begin("ScalerPanel"))
		{
			if (ImGui::TreeNode("Scalers"))
			{
				for (auto& scaler : scalerList)
				{
					ImGui::BulletText("%s", scaler.c_str());
				}
				ImGui::TreePop();
			}
			if (ImGui::BeginCombo("Graph to Plot", m_selectedGraph.name.c_str()))
			{
				for (auto& graph : graphList)
				{
					if (ImGui::Selectable(graph.name.c_str(), m_selectedGraph.name == graph.name))
						m_selectedGraph = graph;
				}
				ImGui::EndCombo();
			}
			if (ImPlot::BeginPlot("Scaler Graphs"))
			{
				manager->DrawGraph(m_selectedGraph.name);
				ImPlot::EndPlot();
			}
		}
		ImGui::End();
	}
}