/*
	Histogram.h
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
#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "NavCore.h"

namespace Navigator {

	enum class SpectrumType
	{
		Histo1D,
		Histo2D,
		Summary,
		None
	};

	std::string ConvertSpectrumTypeToString(SpectrumType type);


	struct NAV_API StatResults
	{
		double integral = 0.0;
		double cent_x = 0.0;
		double cent_y = 0.0;
		double sigma_x = 0.0;
		double sigma_y = 0.0;
	};

	struct NAV_API HistogramParameters
	{
		HistogramParameters() {}

		HistogramParameters(const std::string& n, const std::string& x, int bins, double min, double max) :
			name(n), x_par(x), nbins_x(bins), min_x(min), max_x(max)
		{
		}

		HistogramParameters(const std::string& n, const std::string& x, const std::string& y, int binsx, double minx, double maxx, int binsy, double miny, double maxy) :
			name(n), x_par(x), y_par(y), nbins_x(binsx), min_x(minx), max_x(maxx), nbins_y(binsy), min_y(miny), max_y(maxy)
		{
		}

		SpectrumType type = SpectrumType::None;
		std::string name = "None";
		std::string x_par = "None";
		std::string y_par = "None";
        std::vector<std::string> cutsDrawnUpon;
        std::vector<std::string> cutsAppliedTo;
		int nbins_x = 0;
		double min_x = 0;
		double max_x = 0;
		int nbins_y = 0;
		double min_y = 0;
		double max_y = 0;
	};

	class NAV_API Histogram
	{
	public:
		Histogram() :
			m_initFlag(false)
		{
		}
		Histogram(const HistogramParameters& params) :
			m_params(params), m_initFlag(false)
		{
		}

		virtual ~Histogram() {};
		virtual void FillData(double x, double y=0.0) { NAV_WARN("Trying to fill a default histogram!"); }
		virtual void Draw() {}
		virtual void ClearData() {}
		virtual StatResults AnalyzeRegion(double x_min, double x_max, double y_min = 0.0, double y_max = 0.0) { return StatResults();  }
		inline virtual float* GetColorScaleRange() { return nullptr; }
        inline virtual std::vector<double> GetBinData() { return std::vector<double>(); }
		inline HistogramParameters& GetParameters() { return m_params; }
		inline SpectrumType GetType() { return m_params.type; }
		inline const std::string& GetXParam() const { return m_params.x_par; };
		inline const std::string& GetYParam() const { return m_params.y_par; };
		inline const std::string& GetName() const { return m_params.name; }
        inline void AddCutToBeDrawn(const std::string& name) { m_params.cutsDrawnUpon.push_back(name); }
        inline void AddCutToBeApplied(const std::string& name) { m_params.cutsAppliedTo.push_back(name); }

	protected:
		HistogramParameters m_params;
		bool m_initFlag;
	};

	class NAV_API Histogram1D : public Histogram
	{
	public:
		Histogram1D(const HistogramParameters& params);
		virtual ~Histogram1D();
		virtual void FillData(double x, double y=0.0) override;
		virtual void Draw() override;
		virtual void ClearData() override;
		virtual StatResults AnalyzeRegion(double x_min, double x_max, double y_min = 0.0, double y_max = 0.0) override;
        inline virtual std::vector<double> GetBinData() override { return m_binCounts; }

	private:
		void InitBins();

		std::vector<double> m_binCenters;
		std::vector<double> m_binCounts;
		double m_binWidth;
		
	};

	class NAV_API Histogram2D : public Histogram
	{
	public:
		Histogram2D(const HistogramParameters& params);
		virtual ~Histogram2D();
		virtual void FillData(double x, double y) override;
		virtual void Draw() override;
		virtual void ClearData() override;
		virtual StatResults AnalyzeRegion(double x_min, double x_max, double y_min = 0.0, double y_max = 0.0) override;
        inline virtual std::vector<double> GetBinData() override { return m_binCounts; }

		inline virtual float* GetColorScaleRange() override { return m_colorScaleRange; }

	private:
		void InitBins();

		std::vector<double> m_binCounts;
		int m_nBinsTotal;
		double m_binWidthY;
		double m_binWidthX;
		double m_maxBinContent;
		float m_colorScaleRange[2];
	};

	class NAV_API HistogramSummary : public Histogram
	{
	public:
		HistogramSummary(const HistogramParameters& params, const std::vector<std::string>& subhistos);
		~HistogramSummary();

		inline const std::vector<std::string>& GetSubHistograms() const { return m_subhistos;  }

		virtual void FillData(double x, double y) override;
		virtual void ClearData() override;
		virtual void Draw() override;
		inline virtual float* GetColorScaleRange() override { return m_colorScaleRange; }
		virtual StatResults AnalyzeRegion(double x_min, double x_max, double y_min = 0.0, double y_max = 0.0) override;
		inline virtual std::vector<double> GetBinData() override { return m_binCounts; }

	private:
		void InitBins();

		std::vector<std::string> m_subhistos;
		const char** m_labels;
		std::vector<double> m_binCounts;
		int m_nBinsTotal;
		double m_binWidthX;
		const double m_binWidthY = 1.0;
		float m_colorScaleRange[2];
	};

}

#endif
