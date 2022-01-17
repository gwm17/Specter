#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "NavCore.h"

namespace Navigator {

	struct NAV_API HistogramParameters
	{
		HistogramParameters(const std::string& n, const std::string& x, const std::string& y) :
			name(n), x_par(x), y_par(y)
		{
		}
		std::string name;
		std::string x_par;
		std::string y_par;
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
			m_params("None", "None", "None"), m_initFlag(false)
		{
		}
		Histogram(const std::string& name, const std::string& param_x, const std::string& param_y="None") :
			m_params(name, param_x, param_y), m_initFlag(false)
		{
		}

		virtual ~Histogram() {};
		virtual void FillData(double x, double y = 0) { NAV_WARN("Trying to fill a default histogram!"); }
		virtual void Draw() {}
		virtual void ClearData() {}
		inline virtual bool Is1D() const { return false; }
		inline virtual bool Is2D() const { return false; }
		inline const HistogramParameters& GetParameters() const { return m_params; }
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
		Histogram1D(const std::string& name, const std::string& param, int bins, double min, double max);
		virtual ~Histogram1D();
		virtual void FillData(double x, double y=0) override;
		virtual void Draw() override;
		virtual void ClearData() override;
		inline virtual bool Is1D() const override { return true; }
		inline virtual bool Is2D() const override { return false; }

	private:
		void InitBins(int bins, double min, double max);

		std::vector<double> m_binCenters;
		std::vector<double> m_binCounts;
		double m_binWidth;
		
	};

	class NAV_API Histogram2D : public Histogram
	{
	public:
		Histogram2D(const std::string& name, const std::string& param_x, const std::string& param_y, int bins_x, double min_x, double max_x,
					int bins_y, double min_y, double max_y);
		virtual ~Histogram2D();
		virtual void FillData(double x, double y=0) override;
		virtual void Draw() override;
		virtual void ClearData() override;
		inline virtual bool Is1D() const override { return false; }
		inline virtual bool Is2D() const override { return true; }

	private:
		void InitBins(int bins_x, double min_x, double max_x, int bins_y, double min_y, double max_y);

		std::vector<double> m_binCounts;
		int m_nBinsTotal;
		double m_binWidthY;
		double m_binWidthX;
		double m_maxBinContent;
	};

}

#endif
