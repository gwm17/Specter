#ifndef PARAMETER_MAP_H
#define PARAMETER_MAP_H

namespace Navigator {

	struct ParameterData
	{
		double value = 0;
		bool validFlag = false;
	};

	class NavParameter
	{

	public:
		NavParameter(const std::string& name, const std::string& alias);
		~NavParameter();

		inline void SetValue(double value) { p.value = value; }

	private:
		std::string name;
		ParameterData p;

	};

	class ParameterMap
	{

	public:
		ParameterMap();
		~ParameterMap();
		inline void AddParameter(const std::string& name) { m_map[name] = nullptr; }
		inline void SetParameter(const std::string& name, ParameterData* param) { m_map[name] = param; }
		double GetParameterValue(const std::string& name);
		void ResetParameters();

		inline static ParameterMap& GetParameterMap() { return s_map; }

	private:
		std::unordered_map<std::string, ParameterData*> m_map;
		static ParameterMap s_map;

	};

	ParameterMap* CreateParameterMap();
}

#endif