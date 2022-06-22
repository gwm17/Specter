#ifndef SCALER_PANEL_H
#define SCALER_PANEL_H

#include "Specter/Core/Graph.h"

namespace Specter {

	class ScalerPanel
	{
	public:
		ScalerPanel();
		~ScalerPanel();

		void OnImGuiRender(const std::vector<std::string>& scalerList, const std::vector<GraphArgs>& graphList);

	private:
		GraphArgs m_selectedGraph;
	};

}

#endif