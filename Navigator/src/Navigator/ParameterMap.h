#ifndef PARAMETER_MAP_H
#define PARAMETER_MAP_H

#include "NavCore.h"

namespace Navigator {

	struct NAV_API ParameterData
	{
		double value=0.0;
		bool validFlag=false;
	};

    /*
        For use inside of the physics thread only!!!!!! Do not use elsewhere as complex operations on parameter values are !not!
        guaranteed to be thread-safe, only the accesing is!
     */
	class NAV_API NavParameter
	{

	public:
		NavParameter(const std::string& name, const std::string& alias);
		~NavParameter();

        inline bool IsValid() const { return m_pdata->validFlag; }
        inline void Invalidate() { m_pdata->validFlag = false; }
        inline void SetValue(double value) { m_pdata->validFlag = true; m_pdata->value = value; }
        inline double GetValue() const { return m_pdata->value; }

	private:
		std::string m_name;
		std::shared_ptr<ParameterData> m_pdata;

	};

    /*
        Global parameter accesing, storage
     */
	class NAV_API ParameterMap
	{

	public:
        
        using Iter = std::unordered_map<std::string, std::shared_ptr<ParameterData>>::iterator;
        
		ParameterMap();
		~ParameterMap();
        inline void AddParameter(const std::string& name) { m_map[name] = std::make_shared<ParameterData>(); }
		inline void SetParameter(const std::string& name, std::shared_ptr<ParameterData>& param) { param = m_map[name]; }
		double GetParameterValue(const std::string& name);
        bool IsParameterValid(const std::string& name);
		void InvalidateParameters();
		std::vector<std::string> GetListOfParameters(); //Dangerous! Should only be used when GUARANTEED no phys thread is running.
        inline Iter end() { return m_map.end(); }
        inline Iter begin() { return m_map.begin(); }
        inline Iter find(const std::string& name) { return m_map.find(name); }

		inline static ParameterMap& GetInstance() { return *s_instance; }

	private:
		std::unordered_map<std::string, std::shared_ptr<ParameterData>> m_map;
		static ParameterMap* s_instance;

	};

}

#endif
