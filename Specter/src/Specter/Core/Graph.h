#ifndef GRAPH_H
#define GRAPH_H

namespace Specter {

	struct GraphArgs
	{
		std::string name;
		std::string scalerName;
		size_t maxPoints;
	};

	class ScalerGraph
	{
	public:
		ScalerGraph(const GraphArgs& args);
		~ScalerGraph();

		void UpdatePoints(double timeStep, uint64_t scalerVal);
		void Draw();
		inline void Clear() { m_xPoints.clear(); m_yPoints.clear(); }

		inline const std::string& GetName() const { return m_args.name; }
		inline const std::string& GetScaler() const { return m_args.scalerName; }
		inline const GraphArgs& GetArgs() const { return m_args; }

	private:

		GraphArgs m_args;

		uint64_t m_lastScalerVal;

		std::vector<double> m_yPoints;
		std::vector<double> m_xPoints;
	};

}

#endif