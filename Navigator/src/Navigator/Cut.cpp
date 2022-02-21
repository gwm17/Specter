#include "Cut.h"
#include "implot.h"

namespace Navigator {

	/*1D Cuts -- Can be made on and applied to either 1D or 2D histograms*/
	Cut1D::Cut1D(const CutParams& params, double min, double max) :
		Cut(params), m_minVal(min), m_maxVal(max)
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
    Cut2D::Cut2D(const CutParams& params, const std::vector<double>& xpoints, const std::vector<double>& ypoints) :
        Cut(params), m_xpoints(xpoints), m_ypoints(ypoints)
	{
	}

	Cut2D::~Cut2D() {}

	/*
        Even-odd point in polygon algorithm (see Wikipedia)
        Walk around the sides of the polygon and check intersection with each of  the sides.
        Cast a ray from the point to infinity in any direction and check the number of intersections.
        If odd number of intersections, point is inside. Even, point is outside.
        Edge cases of point is a vertex or on a side considered.
	*/
	bool Cut2D::IsInside(double x, double y) const
	{
        bool result = false;
        double slope;
        for(size_t i=0; i<(m_xpoints.size()-1); i++)
        {
            if(x == m_xpoints[i+1] && y == m_ypoints[i+1])
                return true;
            else if((m_ypoints[i+1] > y) !=  (m_ypoints[i] > y))
            {
                slope = (x - m_xpoints[i+1])*(m_ypoints[i] - m_ypoints[i+1]) - (m_xpoints[i] - m_xpoints[i+1])*(y - m_ypoints[i+1]);
                if(slope == 0.0)
                    return true;
                else if ((slope < 0.0) != (m_ypoints[i] < m_ypoints[i+1]))
                    result = !result;
            }
        }
		return result;
	}

    //Only in ImPlot/ImGui context!!!!
    void Cut2D::Draw() const
    {
        ImPlot::PlotLine(m_params.name.c_str(), m_xpoints.data(), m_ypoints.data(), (int)m_xpoints.size());
    }

}
