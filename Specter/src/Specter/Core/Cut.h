/*
	Cut.h
	Cut related classes. A cut here is defined as a filter upon a histogram based on some range for a parameter or set of parameters.

	CutArgs is the underlying data that defines a cut (excluding the actual points).

	Cut is the base class for all cut objects. Should not be used in practice. All cut objects have functions which can query what kind of cut it is. If one has the cut object,
	Is1D() or Is2D() can be called. If one has the CutArgs, a 1D cut will have y_par set to "None" while a 2D cut will have a valid parameter name.

	Cut1D is a one-dimensional (single parameter cut) while Cut2D is a two-dimensional (two parameter cut). There are a few differences between 1D and 2D cuts. 
	A Cut1D only contains two values, a min and a max. The parameter is checked that it falls within these bounds.
	A Cut2D contains a set of (x,y) points that form a closed polygon. The polygon may be convex. The parameter is checked that it falls within the polygon using similar methods
	to flood-fill algorithms.

	GWM -- Feb 2022
*/
#ifndef CUT_H
#define CUT_H

#include "SpecCore.h"
#include "imgui.h"

namespace Specter {

	enum class CutType
	{
		Cut1D,
		Cut2D,
		CutSummaryAny,
		CutSummaryAll,
		None
	};

	std::string ConvertCutTypeToString(CutType type);
	CutType ConvertStringToCutType(const std::string& keyword);

	struct CutArgs
	{
        CutArgs() {}
		CutArgs(const std::string& n, const std::string& x, const std::string& y = "None") :
			name(n), x_par(x), y_par(y)
		{
		}

		CutType type = CutType::None;
		std::string name = "None";
		std::string x_par = "None";
		std::string y_par = "None";
	};

	class Cut
	{
	public:
        
		Cut(const CutArgs& params) :
			m_params(params), m_isValid(false)
		{
		}

		virtual ~Cut() {}

		virtual void IsInside(double x, double y=0.0) = 0;
		virtual void Draw() const = 0;
		virtual std::vector<double> GetXValues() const = 0;
		virtual std::vector<double> GetYValues() const = 0;

		const bool IsValid() const { return m_isValid; }
		void ResetValidity() { m_isValid = false; }
		CutType GetType() const { return m_params.type;  }
		const std::string& GetName() const { return m_params.name; }
		const std::string& GetXParameter() const { return m_params.x_par; }
		const std::string& GetYParameter() const { return m_params.y_par;  }
        const CutArgs& GetCutArgs() const { return m_params; }
	protected:
		CutArgs m_params;
		bool m_isValid;
	};

	class Cut1D : public Cut
	{
	public:
		Cut1D(const CutArgs& params, double min, double max);
		virtual ~Cut1D();
		virtual void IsInside(double x, double y=0.0) override;
		virtual void Draw() const override;
		virtual std::vector<double> GetXValues() const override { return std::vector<double>({ m_minVal, m_maxVal }); }
		virtual std::vector<double> GetYValues() const override { return std::vector<double>(); }

	private:
		double m_minVal, m_maxVal;
	};

	class Cut2D : public Cut
	{
	public:
		Cut2D(const CutArgs& params, const std::vector<double>& xpoints, const std::vector<double>& ypoints);
		virtual ~Cut2D();
		virtual void IsInside(double x, double y) override;
		virtual void Draw() const override;
		virtual std::vector<double> GetXValues() const override { return m_xpoints; }
		virtual std::vector<double> GetYValues() const override { return m_ypoints; }

	private:
		std::vector<double> m_xpoints;
		std::vector<double> m_ypoints;
	};

	class CutSummary : public Cut
	{
	public:
		CutSummary(const CutArgs& params, const std::vector<std::string>& subhistos, double min, double max);
		virtual ~CutSummary();
		virtual void IsInside(double x, double y) override;
		virtual void Draw() const override;
		virtual std::vector<double> GetXValues() const override { return std::vector<double>({ m_minVal, m_maxVal }); }
		virtual std::vector<double> GetYValues() const override { return std::vector<double>(); }

		const std::vector<std::string>& GetSubHistograms() const { return m_subhistos;  }

	private:
		double m_minVal, m_maxVal;
		std::vector<std::string> m_subhistos;
	};

}

#endif
