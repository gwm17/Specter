#include "CutMap.h"
#include "imgui.h"
#include "implot.h"

namespace Navigator {

	/*1D Cuts -- Can be made on and applied to either 1D or 2D histograms*/
	Cut1D::Cut1D(const std::string& name, const std::string& xpar, double min, double max) :
		Cut(name, xpar), m_minVal(min), m_maxVal(max)
	{
	}

	Cut1D::~Cut1D() {}

	bool Cut1D::IsInside(double x, double y) const
	{
		return x >= m_minVal && x <= m_maxVal;
	}

	//Only within an ImPlot/ImGui context!!!
	void Cut1D::Draw() const
	{
		double points[2] = { m_minVal, m_maxVal };
		ImPlot::PlotVLines(m_params.name.c_str(), points, 2);
	}

	/*2D Cuts -- Can only be made on 2D histogram, but applied to either 1D or 2D histograms*/
	Cut2D::Cut2D(const std::string& name, const std::string& xpar, const std::string& ypar, const std::vector<Point>& points) :
		Cut(name, xpar, ypar), m_points(points)
	{
	}

	Cut2D::~Cut2D() {}

	/*
	
	*/
	bool Cut2D::IsInside(double x, double y) const
	{
		return false;
	}
}