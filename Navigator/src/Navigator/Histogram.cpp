#include "Histogram.h"
#include "implot.h"

/*
	For Navigator histograms, binnning is done over ranges as
	[min, max), so that each bin is exclusive. 
*/

namespace Navigator {

	/*
		1D Histogram class
	*/
	Histogram1D::Histogram1D(const std::string& name, const std::string& param, int bins, double min, double max) :
		Histogram(name, param), m_nBins(bins), m_xMin(min), m_xMax(max)
	{
		InitBins();
	}

	Histogram1D::~Histogram1D() {}

	void Histogram1D::InitBins()
	{
		if(m_nBins == 0 || (m_xMin >= m_xMax))
		{
			NAV_WARN("Attempting to create an illegal Histogram1D {0} with {1} bins and a range from {2} to {3}. Historgram not initialized.", m_name, m_nBins, m_xMin, m_xMax);
			m_initFlag = false;
			return;
		}

		m_binWidth = (m_xMax - m_xMin)/m_nBins;

		m_binCenters.resize(m_nBins);
		m_binCounts.resize(m_nBins);

		for(int i=0; i<m_nBins; i++)
		{
			m_binCenters[i] = m_xMin + i*m_binWidth + m_binWidth*0.5;
			m_binCounts[i] = 0;
		}

		m_initFlag = true;
	}

	//Note: only x is used here, y is simply present to maintain compliance with 2D case and can be ignored
	void Histogram1D::FillData(double x, double y)
	{
		if(x < m_xMin || x >= m_xMax || !m_initFlag)
			return;

		int bin = int((x - m_xMin)/(m_binWidth));

		m_binCounts[bin] += 1.0;
	}

	//Can only be used within an ImGui / ImPlot context!!
	void Histogram1D::Draw()
	{
		ImPlot::PlotBars(m_name.c_str(), &m_binCenters.data()[0], &m_binCounts.data()[0], m_nBins, m_binWidth);
	}

	void Histogram1D::ClearData()
	{
		for(int i=0; i<m_nBins; i++)
			m_binCounts[i] = 0;
	}


	/*
		2D Histogram class
	*/
	Histogram2D::Histogram2D(const std::string& name, const std::string& param_x, const std::string& param_y, int bins_x, double min_x, double max_x,
							 int bins_y, double min_y, double max_y) :
		Histogram(name, param_x, param_y), m_nXBins(bins_x), m_nYBins(bins_y), m_xMin(min_x), m_xMax(max_x), m_yMin(min_y), m_yMax(max_y)
	{
		InitBins();
	}

	Histogram2D::~Histogram2D() {}

	void Histogram2D::InitBins()
	{
		if(m_nXBins == 0 || m_nYBins == 0 || m_xMin >= m_xMax || m_yMin >= m_yMax)
		{
			NAV_WARN("Attempting to create illegal Histogram2D {0} with {1} x bins, {2} y bins, an x range of {3} to {4}, and a y range of {5} to {6}. Not initialized.", m_name, m_nXBins, m_nYBins,
					 m_xMin, m_xMax, m_yMin, m_yMax);
			m_initFlag = false;
			return;
		}

		m_binWidthX = (m_xMax - m_xMin)/m_nXBins;
		m_binWidthY = (m_yMax - m_yMin)/m_nYBins;

		m_nBins = m_nXBins*m_nYBins;

		m_binCounts.resize(m_nBins);
		for(int i=0; i<m_nBins; i++)
			m_binCounts[i] = 0;
		m_maxBinContent = 0;
	}

	void Histogram2D::FillData(double x, double y)
	{
		if(x < m_xMin || x >= m_xMax || y < m_yMin || y >= m_yMax || !m_initFlag)
			return;

		int bin_x = int((x - m_xMin)/m_binWidthX);
		int bin_y = int((y - m_yMin)/m_binWidthY);
		int bin = bin_y*m_nXBins + bin_x;

		m_binCounts[bin] += 1.0;

		m_maxBinContent = m_binCounts[bin] > m_maxBinContent ? m_binCounts[bin] : m_maxBinContent;
	}

	//Can only be used within an ImGui / ImPlot context!!
	void Histogram2D::Draw()
	{
		ImPlot::PlotHeatmap(m_name.c_str(), &m_binCounts.data()[0], m_nYBins, m_nXBins, 0, m_maxBinContent, NULL, ImPlotPoint(m_xMin, m_yMin), ImPlotPoint(m_xMax, m_yMax));
	}

	void Histogram2D::ClearData()
	{
		for(int i=0; i<m_nBins; i++)
			m_binCounts[i] = 0;
		m_maxBinContent = 0;
	}
}