#include "Histogram.h"
#include "CutMap.h"
#include "implot.h"

/*
	For Navigator histograms, binnning is done over ranges as
	[min, max), so that each bin is exclusive. 
*/

namespace Navigator {

	/*
		1D Histogram class
	*/
	Histogram1D::Histogram1D(const HistogramParameters& params) :
		Histogram(params)
	{
		InitBins();
	}

	Histogram1D::~Histogram1D() {}

	void Histogram1D::InitBins()
	{
		if(m_params.nbins_x == 0 || (m_params.min_x >= m_params.max_x))
		{
			NAV_WARN("Attempting to create an illegal Histogram1D {0} with {1} bins and a range from {2} to {3}. Historgram not initialized.", m_params.name, m_params.nbins_x, m_params.min_x, m_params.max_x);
			m_initFlag = false;
			return;
		}

		m_binWidth = (m_params.max_x - m_params.min_x)/m_params.nbins_x;

		m_binCenters.resize(m_params.nbins_x);
		m_binCounts.resize(m_params.nbins_x);

		for(int i=0; i<m_params.nbins_x; i++)
		{
			m_binCenters[i] = m_params.min_x + i*m_binWidth + m_binWidth*0.5;
			m_binCounts[i] = 0;
		}

		m_initFlag = true;
	}

	//Note: only x is used here, y is simply present to maintain compliance with 2D case and can be ignored
	void Histogram1D::FillData(double x, double y)
	{
		if(x < m_params.min_x || x >= m_params.max_x || !m_initFlag)
			return;

		int bin = int((x - m_params.min_x)/(m_binWidth));
        
        auto& cutmap = CutMap::GetInstance();

		m_binCounts[bin] += 1.0;
	}

	//Can only be used within an ImGui / ImPlot context!!
	void Histogram1D::Draw()
	{
		ImPlot::SetupAxes(m_params.x_par.c_str(), "Counts",0, ImPlotAxisFlags_LockMin);
		ImPlot::PlotBars(m_params.name.c_str(), &m_binCenters.data()[0], &m_binCounts.data()[0], m_params.nbins_x, m_binWidth);
        auto& cutmap = CutMap::GetInstance();
        for(auto& cut : m_params.cutsDrawnUpon)
            cutmap.DrawCut(cut);
	}

	void Histogram1D::ClearData()
	{
		for(int i=0; i<m_params.nbins_x; i++)
			m_binCounts[i] = 0;
	}


	/*
		2D Histogram class
	*/
	Histogram2D::Histogram2D(const HistogramParameters& params) :
		Histogram(params)
	{
		InitBins();
	}

	Histogram2D::~Histogram2D() {}

	void Histogram2D::InitBins()
	{
		if(m_params.nbins_x <= 0 || m_params.nbins_y <= 0 || m_params.min_x >= m_params.max_x || m_params.min_y >= m_params.max_y)
		{
			NAV_WARN("Attempting to create illegal Histogram2D {0} with {1} x bins, {2} y bins, an x range of {3} to {4}, and a y range of {5} to {6}. Not initialized.", m_params.name, m_params.nbins_x, m_params.nbins_y,
					 m_params.min_x, m_params.max_x, m_params.min_y, m_params.max_y);
			m_initFlag = false;
			return;
		}

		m_binWidthX = (m_params.max_x - m_params.min_x)/m_params.nbins_x;
		m_binWidthY = (m_params.max_y - m_params.min_y)/m_params.nbins_y;

		m_nBinsTotal = m_params.nbins_x*m_params.nbins_y;

		m_binCounts.resize(m_nBinsTotal);
		for(int i=0; i<m_nBinsTotal; i++)
			m_binCounts[i] = 0;
		m_maxBinContent = 0;

		m_initFlag = true;
	}

	void Histogram2D::FillData(double x, double y)
	{
		if(x < m_params.min_x || x >= m_params.max_x || y < m_params.min_y || y >= m_params.max_y || !m_initFlag)
			return;

		int bin_x = int((x - m_params.min_x)/m_binWidthX);
		int bin_y = int((m_params.max_y - y)/m_binWidthY);
		int bin = bin_y*m_params.nbins_x + bin_x;

		m_binCounts[bin] += 1.0;

		m_maxBinContent = m_binCounts[bin] > m_maxBinContent ? (m_binCounts[bin]*2) : m_maxBinContent;
	}

	//Can only be used within an ImGui / ImPlot context!!
	void Histogram2D::Draw()
	{
		ImPlot::SetupAxes(m_params.x_par.c_str(), m_params.y_par.c_str());
		ImPlot::PushColormap(ImPlotColormap_Viridis);
		ImPlot::PlotHeatmap(m_params.name.c_str(), &m_binCounts.data()[0], m_params.nbins_y, m_params.nbins_x, 0.0, m_maxBinContent, NULL,
							ImPlotPoint(m_params.min_x, m_params.min_y), ImPlotPoint(m_params.max_x, m_params.max_y));
		ImPlot::PopColormap();
        auto& cutmap = CutMap::GetInstance();
        for(auto& cut : m_params.cutsDrawnUpon)
            cutmap.DrawCut(cut);
	}

	void Histogram2D::ClearData()
	{
		for(int i=0; i<m_nBinsTotal; i++)
			m_binCounts[i] = 0;
		m_maxBinContent = 0;
	}
}
