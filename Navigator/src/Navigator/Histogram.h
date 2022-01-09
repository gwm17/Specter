#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "NavCore.h"

namespace Navigator {

	class NAV_API Histogram
	{
	public:
		Histogram(const std::string& name, const std::string& param_x, const std::string& param_y="None") :
			m_name(name), m_xParam(param_x), m_yParam(param_y), m_initFlag(false)
		{
		}

		virtual ~Histogram() {};
		virtual void FillData(double x, double y=0) = 0;
		virtual void Draw() = 0;
		virtual void ClearData() = 0;
		inline const std::string& GetXParam() const { return m_xParam; };
		inline const std::string& GetYParam() const { return m_yParam; };
		inline const std::string& GetName() const { return m_name; }

	protected:
		std::string m_name;
		std::string m_xParam;
		std::string m_yParam;
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
		

	private:
		void InitBins();

		std::vector<double> m_binCenters;
		std::vector<double> m_binCounts;
		int m_nBins;
		double m_binWidth;
		double m_xMin;
		double m_xMax;
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

	private:
		void InitBins();

		std::vector<double> m_binCounts;
		int m_nBins;
		int m_nXBins;
		int m_nYBins;
		double m_xMin;
		double m_xMax;
		double m_yMin;
		double m_yMax;
		double m_binWidthY;
		double m_binWidthX;
		double m_maxBinContent;
	};

}

#endif