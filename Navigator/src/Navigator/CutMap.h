#ifndef CUT_MAP_H
#define CUT_MAP_H

#include "NavCore.h"
#include "imgui.h"

namespace Navigator {

	struct NAV_API CutParams
	{
        CutParams() {}
		CutParams(const std::string& n, const std::string& x, const std::string& y = "None") :
			name(n), x_par(x), y_par(y)
		{
		}

		std::string name = "None";
		std::string x_par = "None";
		std::string y_par = "None";
	};

	class NAV_API Cut
	{
	public:
        
		Cut(const CutParams& params) :
			m_params(params)
		{
		}

		virtual ~Cut() {}

		virtual bool IsInside(double x, double y = 0) const = 0;
		virtual void Draw() const = 0;
		virtual bool Is1D() const = 0;
		virtual bool Is2D() const = 0;

		inline const std::string& GetName() const { return m_params.name; }
		inline const std::string& GetXParameter() const { return m_params.x_par; }
		inline const std::string& GetYParameter() const { return m_params.y_par;  }
        inline const CutParams& GetCutParams() const { return m_params; }
	protected:
		CutParams m_params;
	};

	class NAV_API Cut1D : public Cut
	{
	public:
		Cut1D(const CutParams& params, double min, double max);
		virtual ~Cut1D();
		virtual bool IsInside(double x, double y = 0) const override;
		virtual void Draw() const override;
		virtual bool Is1D() const override { return true; }
		virtual bool Is2D() const override { return false; }

	private:
		double m_minVal, m_maxVal;
	};

	class NAV_API Cut2D : public Cut
	{
	public:
		Cut2D(const CutParams& params, const std::vector<double>& xpoints, const std::vector<double>& ypoints);
		virtual ~Cut2D();
		virtual bool IsInside(double x, double y = 0) const override;
		virtual void Draw() const override;
		virtual bool Is1D() const override { return false; }
		virtual bool Is2D() const override { return true; }

	private:
		std::vector<double> m_xpoints;
		std::vector<double> m_ypoints;
        const ImVec4 colorVec = {1.0, 0.0, 0.0, 0.5};
	};

	class NAV_API CutMap
	{
	public:
		using Iter = std::unordered_map<std::string, std::shared_ptr<Cut>>::iterator;

		CutMap();
		~CutMap();

		inline static CutMap& GetInstance() { return *s_instance; }

		inline void AddCut(const CutParams& params, double min, double max)
		{
			m_map[params.name].reset(new Cut1D(params, min, max));
		}
		inline void AddCut(const CutParams& params, const std::vector<double>& xpoints, const std::vector<double>& ypoints)
		{
			m_map[params.name].reset(new Cut2D(params, xpoints, ypoints));
		}

		void DrawCut(const std::string& name);
		bool IsInsideCut(const std::string& name, double xval, double yval = 0);
		std::vector<CutParams> GetListOfCutParams();

		inline Iter begin() { return m_map.begin(); }
		inline Iter end() { return m_map.end(); }

	private:
		std::unordered_map<std::string, std::shared_ptr<Cut>> m_map;

		static CutMap* s_instance;
	};
}

#endif
