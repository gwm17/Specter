#include "CutMap.h"
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
     Even-odd point in polygon algorithm (see Wikipedia)
     Walk around the sides of the polygon and check intersection with each of  the sides.
     Cast a ray from the point to infinity in any direction and check the number of intersections.
     If odd number of intersections, point is inside. Even, point is outside.
     Edge cases of point is a vertex or on a side considered.
	*/
	bool Cut2D::IsInside(double x, double y) const
	{
        bool result = false;
        size_t j = m_points.size() -1;
        double slope;
        for(size_t i=0; i<m_points.size(); i++)
        {
            if(x == m_points[i].x && y == m_points[i].y)
                return true;
            else if((m_points[i].y > y) !=  (m_points[j].y > y))
            {
                slope = (x - m_points[i].x)*(m_points[j].y - m_points[i].y) - (m_points[j].x - m_points[i].x)*(y - m_points[i].y);
                if(slope == 0.0)
                    return true;
                else if ((slope < 0.0) != (m_points[j].y < m_points[i].y))
                    result = !result;
            }
            j=i;
        }
		return result;
	}

    //Only in ImPlot/ImGui context!!!!
    /*
      Again, more complicated. Want to draw the polygon that makes up the 2D-cut. To perform this,
      first need to convert plot coordinates into global window coords (pixels). Then tell ImGui to add
      a polyline to the draw list, make it red and closed. For this to work, this must be called within the
      BeignPlot()/EndPlot() combo for which it was  created (why histos own the draw commands).
     */
    void Cut2D::Draw() const
    {
        std::vector<ImVec2> draw_points;
        draw_points.reserve(m_points.size());
        auto draw_list = ImGui::GetWindowDrawList();
        
        for(auto& point : m_points)
            draw_points.push_back(ImPlot::PlotToPixels(point.x, point.y));
        
        draw_list->AddPolyline(draw_points.data(), static_cast<int>(draw_points.size()), ImGui::ColorConvertFloat4ToU32(colorVec), ImDrawFlags_Closed, 0.1);
    }

    /* CutMap */
    CutMap* CutMap::s_instance = new CutMap(); //Guarantee existance for duration of program.

    CutMap::CutMap() {}

    CutMap::~CutMap() {}

    void CutMap::DrawCut(const std::string& name)
    {
        std::lock_guard<std::mutex> lock(m_cutMutex);
        auto iter = m_map.find(name);
        if(iter != m_map.end())
            iter->second->Draw();
    }

    bool CutMap::IsInsideCut(const std::string& name, double xval, double yval)
    {
        std::lock_guard<std::mutex> lock(m_cutMutex);
        bool result = false;
        auto iter = m_map.find(name);
        if(iter != m_map.end())
            result = iter->second->IsInside(xval, yval);
        return result;
    }

    std::vector<CutParams> CutMap::GetListOfCutParams()
    {
        std::lock_guard<std::mutex> lock(m_cutMutex);
        std::vector<CutParams> list;
        list.reserve(m_map.size());
        for(auto& entry : m_map)
            list.push_back(entry.second->GetCutParams());
        return list;
    }
}
