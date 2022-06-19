#include "Graph.h"
#include "implot.h"

namespace Specter {

	ScalerGraph::ScalerGraph(const GraphArgs& args) :
		m_args(args), m_lastScalerVal(0)
	{
		m_xPoints.reserve(m_args.maxPoints);
		m_xPoints.reserve(m_args.maxPoints);
	}

	ScalerGraph::~ScalerGraph() {}

	void ScalerGraph::UpdatePoints(double timeStep, uint64_t scalerVal)
	{
		double rate = (scalerVal - m_lastScalerVal) / timeStep;
		if (m_xPoints.size() < m_args.maxPoints)
		{
			m_xPoints.push_back(timeStep);
			m_yPoints.push_back(rate);
		}
		else
		{
			for (size_t i = 0; i < (m_args.maxPoints - 1); i++)
			{
				m_xPoints[i] = m_xPoints[i + 1];
				m_yPoints[i] = m_yPoints[i + 1];
			}
			m_xPoints[m_args.maxPoints - 1] = timeStep;
			m_yPoints[m_args.maxPoints - 1] = rate;
		}

		m_lastScalerVal = scalerVal;
	}

	void ScalerGraph::Draw()
	{
		ImPlot::SetupAxes("time (s)", "rate (Hz)");
		ImPlot::PlotLine(m_args.name.c_str(), m_xPoints.data(), m_yPoints.data(), m_xPoints.size());
	}
}