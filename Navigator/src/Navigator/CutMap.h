#ifndef CUT_MAP_H
#define CUT_MAP_H

#include "NavCore.h"
#include "imgui.h"
#include <thread>

namespace Navigator {

	struct NAV_API CutParams
	{
		CutParams(const std::string& n, const std::string& x, const std::string& y) :
			name(n), x_par(x), y_par(y)
		{
		}

		std::string name;
		std::string x_par;
		std::string y_par;
	};

	class NAV_API Cut
	{
	public:
		Cut(const std::string& name, const std::string& xpar, const std::string& ypar="None") :
			m_params(name, xpar, ypar)
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
		Cut1D(const std::string& name, const std::string& xpar, double min, double max);
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
		Cut2D(const std::string& name, const std::string& xpar, const std::string& ypar, const std::vector<double>& xpoints, const std::vector<double>& ypoints);
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
		CutMap();
		~CutMap();

		inline static CutMap& GetInstance() { return *s_instance; }

		inline void AddCut(const std::string& name, const std::string& xpar, double min, double max)
		{
			std::lock_guard<std::mutex> guard(m_cutMutex);
			m_map[name].reset(new Cut1D(name, xpar, min, max));
		}
		inline void AddCut(const std::string& name, const std::string& xpar, const std::string& ypar, const std::vector<double>& xpoints, const std::vector<double>& ypoints)
		{
			std::lock_guard<std::mutex> guard(m_cutMutex);
			m_map[name].reset(new Cut2D(name, xpar, ypar, xpoints, ypoints));
		}

		void DrawCut(const std::string& name);
		bool IsInsideCut(const std::string& name, double xval, double yval = 0);
		std::vector<CutParams> GetListOfCutParams();

	private:
		std::unordered_map<std::string, std::unique_ptr<Cut>> m_map;
		std::mutex m_cutMutex;

		static CutMap* s_instance;
	};
}

#endif
