#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "NavCore.h"

namespace Navigator {

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
		inline virtual bool Is1D() const { return false; }
		inline virtual bool Is2D() const { return false; }
		inline HistogramParameters& GetParameters() { return m_params; }
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
		inline virtual bool Is1D() const override { return true; }
		inline virtual bool Is2D() const override { return false; }

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
		inline virtual bool Is1D() const override { return false; }
		inline virtual bool Is2D() const override { return true; }

	private:
		void InitBins();

		std::vector<double> m_binCounts;
		int m_nBinsTotal;
		double m_binWidthY;
		double m_binWidthX;
		double m_maxBinContent;
	};

}

#endif
