/*
	Histogram.cpp
	Histogram related classes. We use a custom histogram class here because the ImPlot histograms are not a data structure, but rather just a function. This means that at every draw call for an ImPlot
	histogram the entire data set will need to be properly binned and memory will need to be allocated to make histogram arrays. For our use case this is obviously bad. For one thing, data runs can have
	thousands to millions of events. In the ImPlot paradigm we would need to loop over all of this data and bin it, not to mention explicitly store all of this data in memory for every histogram. I point this
	out not to say that ImPlot histograms are bad intrinsically, because they definitely have a use for smaller data sets, but rather to explain why for this program I have re-invented the wheel somewhat.

	HistogramParameters are the underlying data which define a histogram. This is grouped in a struct to easily pass these around for use in contexts like the Editor.
	Every histogram has a set of histogram parameters.

	Histogram is the base class of all histograms. Should not be used in practice. Every histogram contains functions to query what type of underlying histogram it is. If one has
	the Histogram object, Is1D() or Is2D() can be called. If one only has the HistogramParameters, the values of x_par and y_par can be inspected. In particular, a 1D histogram will have
	y_par set to "None", while a 2D histogram should have a valid parameter name for y_par.

	Histogram1D is a one dimensional (single parameter) histogram. Histogram2D is a two dimensional (two parameter) histogram. The only real difference between these in practice, other than
	the obvious two vs. one parameter thing, is that a Histogram2D contains methods to set the z-axis range (color scale) which ImPlot does not provide intrinsic access to from the plot itself.
	When the range is set to (0,0), the color scale is set to the default (0, maxValue). Otherwise the color is scaled as appropriate. If you query a Histogram1D for its color scale you will recieve
	a nullptr.

	StatResults is a struct containing statistical information about a region of a histogram.

	GWM -- Feb 2022
*/
#include "Histogram.h"
#include "implot.h"

/*
	For Navigator histograms, binnning is done over ranges as
	[min, max), so that each bin is exclusive. 
*/

namespace Navigator {

	std::string ConvertSpectrumTypeToString(SpectrumType type)
	{
		switch (type)
		{
		case SpectrumType::Histo1D: return "Histogram1D";
		case SpectrumType::Histo2D: return "Histogram2D";
		case SpectrumType::Summary: return "Summary";
		case SpectrumType::None: return "None";
		}
		return "None";
	}

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
		m_params.type = SpectrumType::Histo1D;
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
		if (x < m_params.min_x || x >= m_params.max_x)
			return;
		int bin = int((x - m_params.min_x)/(m_binWidth));
		m_binCounts[bin] += 1.0;
	}

	//Can only be used within an ImGui / ImPlot context!!
	void Histogram1D::Draw()
	{
		ImPlot::SetupAxes(m_params.x_par.c_str(), "Counts",0, ImPlotAxisFlags_LockMin | ImPlotAxisFlags_AutoFit);
		ImPlot::PlotBars(m_params.name.c_str(), &m_binCenters.data()[0], &m_binCounts.data()[0], m_params.nbins_x, m_binWidth);
	}

	void Histogram1D::ClearData()
	{
		for(int i=0; i<m_params.nbins_x; i++)
			m_binCounts[i] = 0;
	}

	//Again here yvalues can be ignored, only for compliance
	StatResults Histogram1D::AnalyzeRegion(double x_min, double x_max, double y_min, double y_max)
	{
		int bin_min, bin_max;
		StatResults results;

		//We clamp to the boundaries of the histogram
		if (x_min <= m_params.min_x)
			bin_min = 0;
		else
			bin_min = int((x_min - m_params.min_x) / (m_binWidth));

		if (x_max >= m_params.max_x)
			bin_max = m_params.nbins_x - 1;
		else
			bin_max = int((x_max - m_params.min_x) / (m_binWidth));

		for (int i = bin_min; i <= bin_max; i++)
		{
			results.integral += m_binCounts[i];
			results.cent_x += m_binCounts[i] * (m_params.min_x + m_binWidth * i);
		}
		if (results.integral == 0)
			return results;

		results.cent_x /= results.integral;
		for (int i = bin_min; i <= bin_max; i++)
			results.sigma_x += m_binCounts[i] * ((m_params.min_x + m_binWidth * i) - results.cent_x) * ((m_params.min_x + m_binWidth * i) - results.cent_x);
		results.sigma_x = std::sqrt(results.sigma_x / (results.integral - 1));
		return results;
	}

	/*
		2D Histogram class
		Note for 2D: Rendering is done from top left to bottom right. So ybins run from top to bottom (ymin is last row, ymax is first row)
	*/
	Histogram2D::Histogram2D(const HistogramParameters& params) :
		Histogram(params)
	{
		m_colorScaleRange[0] = 0.0f;
		m_colorScaleRange[1] = 0.0f;
		InitBins();
	}

	Histogram2D::~Histogram2D() {}

	void Histogram2D::InitBins()
	{
		m_params.type = SpectrumType::Histo2D;
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
		if (x < m_params.min_x || x >= m_params.max_x || y <= m_params.min_y || y > m_params.max_y)
			return;
		int bin_x = int((x - m_params.min_x)/m_binWidthX);
		int bin_y = int((m_params.max_y - y)/m_binWidthY);
		int bin = bin_y*m_params.nbins_x + bin_x;

		m_binCounts[bin] += 1.0;

		m_maxBinContent = m_binCounts[bin] > m_maxBinContent ? (m_binCounts[bin]) : m_maxBinContent;
	}

	//Can only be used within an ImGui / ImPlot context!!
	/*
		Brief note on colormaps: There are several kinds of colormaps, each with specific use cases. But broadly, the two main categories are discrete and continuous.
		Discrete maps are good for categorical data, and we use these as the standard for plots we draw. This is how you make cuts standout from your histograms. 
		But for the colors of the bins, we want a continuous scale, i.e. smooth variation on a single hue, as the color differences represent the value differences
		relative to other bins. Hence the pushing of the Matplotlib virdis colormap. For more on colormaps, I suggest looking at matplotlib documentation.
	*/
	void Histogram2D::Draw()
	{
		ImPlot::SetupAxes(m_params.x_par.c_str(), m_params.y_par.c_str());
		ImPlot::PushColormap(ImPlotColormap_Viridis);
		ImPlot::PlotHeatmap(m_params.name.c_str(), &m_binCounts.data()[0], m_params.nbins_y, m_params.nbins_x, m_colorScaleRange[0], m_colorScaleRange[1], NULL,
							ImPlotPoint(m_params.min_x, m_params.min_y), ImPlotPoint(m_params.max_x, m_params.max_y));
		ImPlot::PopColormap();
	}

	void Histogram2D::ClearData()
	{
		for(int i=0; i<m_nBinsTotal; i++)
			m_binCounts[i] = 0;
		m_maxBinContent = 0;
	}

	StatResults Histogram2D::AnalyzeRegion(double x_min, double x_max, double y_min, double y_max)
	{
		int xbin_min, xbin_max, ybin_min, ybin_max;
		int curbin;

		StatResults results;

		//We clamp to the boundaries of the histogram
		if (x_min <= m_params.min_x)
			xbin_min = 0;
		else
			xbin_min = int((x_min - m_params.min_x) / (m_binWidthX));

		if (x_max >= m_params.max_x)
			xbin_max = m_params.nbins_x - 1;
		else
			xbin_max = int((x_max - m_params.min_x) / (m_binWidthX));

		if (y_min <= m_params.min_y)
			ybin_max = m_params.nbins_y - 1;
		else
			ybin_max = int((m_params.max_y - y_min) / m_binWidthY);

		if (y_max >= m_params.max_y)
			ybin_min = 0;
		else
			ybin_min = int((m_params.max_y - y_max) / m_binWidthY);

		for (int y = ybin_min; y <= ybin_max; y++)
		{
			for (int x = xbin_min; x <= xbin_max; x++)
			{
				curbin = y * m_params.nbins_x + x;
				results.integral += m_binCounts[curbin];
				results.cent_x += m_binCounts[curbin] * (m_params.min_x + m_binWidthX * x);
				results.cent_y += m_binCounts[curbin] * (m_params.max_y - m_binWidthY * y);
			}
		}

		if (results.integral == 0)
			return results;

		results.cent_x /= results.integral;
		results.cent_y /= results.integral;
		for (int y = ybin_min; y <= ybin_max; y++)
		{
			for (int x = xbin_min; x <= xbin_max; x++)
			{
				curbin = y * m_params.nbins_x + x;
				results.sigma_x += m_binCounts[curbin] * ((m_params.min_x + m_binWidthX * x) - results.cent_x) * ((m_params.min_x + m_binWidthX * x) - results.cent_x);
				results.sigma_y += m_binCounts[curbin] * ((m_params.max_y - m_binWidthY * y) - results.cent_y) * ((m_params.max_y - m_binWidthY * y) - results.cent_y);
			}
		}

		results.sigma_x = std::sqrt(results.sigma_x / (results.integral - 1));
		results.sigma_y = std::sqrt(results.sigma_y / (results.integral - 1));
		
		return results;
	}

	/*
		HistogramSummary class methods

		-- 03/18/22 Adding in the basics, unsure of how to exactly approach design

		-- Fill data to independent histogram? Steal data from other histograms?

		-- Cuts?

		-- Literally everything hahaha
	*/

	HistogramSummary::HistogramSummary(const HistogramParameters& params, const std::vector<std::string>& subhistos) :
		Histogram(params), m_subhistos(subhistos), m_labels(nullptr)
	{
		m_colorScaleRange[0] = 0.0f;
		m_colorScaleRange[1] = 0.0f;
		InitBins();
	}

	HistogramSummary::~HistogramSummary()
	{
		if (m_labels)
			delete[] m_labels;
	}

	void HistogramSummary::InitBins()
	{
		m_params.type = SpectrumType::Summary;
		if (m_params.nbins_x <= 0 || m_params.min_x >= m_params.max_x)
		{
			NAV_WARN("Attempting to create illegal HistogramSummary {0} with {1} x bins and an x range of {2} to {3}. Not initialized.", m_params.name, m_params.nbins_x, m_params.min_x, m_params.max_x);
			m_initFlag = false;
			return;
		}
		
		m_labels = new const char* [m_subhistos.size() + 1];
		for (size_t i = 0; i < m_subhistos.size(); i++)
			m_labels[i] = m_subhistos[i].c_str();
		m_labels[m_subhistos.size()] = "";
		m_params.nbins_y = m_subhistos.size();
		m_params.min_y = 0.0;
		m_params.max_y = m_subhistos.size();
		m_binWidthX = (m_params.max_x - m_params.min_x) / m_params.nbins_x;

		m_nBinsTotal = m_params.nbins_x * m_params.nbins_y;

		m_binCounts.resize(m_nBinsTotal);
		for (int i = 0; i < m_nBinsTotal; i++)
			m_binCounts[i] = 0;

		m_initFlag = true;
	}

	void HistogramSummary::FillData(double x, double y)
	{
		if (x < m_params.min_x || x >= m_params.max_x || y <= m_params.min_y || y > m_params.max_y)
			return;
		int bin_x = int((x - m_params.min_x) / m_binWidthX);
		int bin_y = int((m_params.max_y - y) / m_binWidthY);
		int bin = bin_y * m_params.nbins_x + bin_x;

		m_binCounts[bin] += 1.0;
	}

	void HistogramSummary::Draw()
	{
		ImPlot::SetupAxisTicks(ImAxis_Y1, m_params.min_y, m_params.max_y, m_params.nbins_y, m_labels, false);
		ImPlot::PushColormap(ImPlotColormap_Viridis);
		ImPlot::PlotHeatmap(m_params.name.c_str(), &m_binCounts.data()[0], m_params.nbins_y, m_params.nbins_x, m_colorScaleRange[0], m_colorScaleRange[1], NULL,
			ImPlotPoint(m_params.min_x, m_params.min_y), ImPlotPoint(m_params.max_x, m_params.max_y));
		ImPlot::PopColormap();
	}

	void HistogramSummary::ClearData()
	{
		for (int i = 0; i < m_nBinsTotal; i++)
			m_binCounts[i] = 0;
	}

	StatResults HistogramSummary::AnalyzeRegion(double x_min, double x_max, double y_min, double y_max)
	{
		int xbin_min, xbin_max, ybin_min, ybin_max;
		int curbin;

		StatResults results;

		//We clamp to the boundaries of the histogram
		if (x_min <= m_params.min_x)
			xbin_min = 0;
		else
			xbin_min = int((x_min - m_params.min_x) / (m_binWidthX));

		if (x_max >= m_params.max_x)
			xbin_max = m_params.nbins_x - 1;
		else
			xbin_max = int((x_max - m_params.min_x) / (m_binWidthX));

		if (y_min <= m_params.min_y)
			ybin_max = m_params.nbins_y - 1;
		else
			ybin_max = int((m_params.max_y - y_min) / m_binWidthY);

		if (y_max >= m_params.max_y)
			ybin_min = 0;
		else
			ybin_min = int((m_params.max_y - y_max) / m_binWidthY);

		for (int y = ybin_min; y <= ybin_max; y++)
		{
			for (int x = xbin_min; x <= xbin_max; x++)
			{
				curbin = y * m_params.nbins_x + x;
				results.integral += m_binCounts[curbin];
				results.cent_x += m_binCounts[curbin] * (m_params.min_x + m_binWidthX * x);
				results.cent_y += m_binCounts[curbin] * (m_params.max_y - m_binWidthY * y);
			}
		}

		if (results.integral == 0)
			return results;

		results.cent_x /= results.integral;
		results.cent_y /= results.integral;
		for (int y = ybin_min; y <= ybin_max; y++)
		{
			for (int x = xbin_min; x <= xbin_max; x++)
			{
				curbin = y * m_params.nbins_x + x;
				results.sigma_x += m_binCounts[curbin] * ((m_params.min_x + m_binWidthX * x) - results.cent_x) * ((m_params.min_x + m_binWidthX * x) - results.cent_x);
				results.sigma_y += m_binCounts[curbin] * ((m_params.max_y - m_binWidthY * y) - results.cent_y) * ((m_params.max_y - m_binWidthY * y) - results.cent_y);
			}
		}

		results.sigma_x = std::sqrt(results.sigma_x / (results.integral - 1));
		results.sigma_y = std::sqrt(results.sigma_y / (results.integral - 1));

		return results;
	}
}
