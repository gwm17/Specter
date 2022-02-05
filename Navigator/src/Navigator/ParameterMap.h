#ifndef PARAMETER_MAP_H
#define PARAMETER_MAP_H

#include "NavCore.h"
#include <thread>

namespace Navigator {

	struct NAV_API ParameterData
	{
		double value=0.0;
		bool validFlag=false;
	};

	class NAV_API NavParameter
	{

	public:
        NavParameter();
		NavParameter(const std::string& name);
		~NavParameter();
        
        void SetParameter(const std::string& name);

        inline bool IsValid() const { return m_pdata->validFlag; }
        inline void Invalidate() { m_pdata->validFlag = false; }
        inline void SetValue(double value) { m_pdata->validFlag = true; m_pdata->value = value; }
        inline double GetValue() const { return m_pdata->value; }
        inline const std::string& GetName() const { return m_name; }

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
		void SetParameter(const std::string& name, std::shared_ptr<ParameterData>& param);
		ParameterData GetParameter(const std::string& name);
        bool IsParameterValid(const std::string& name);
		void InvalidateParameters();
		std::vector<std::string> GetListOfParameters();

		inline static ParameterMap& GetInstance() { return *s_instance; }

	private:
		std::unordered_map<std::string, std::shared_ptr<ParameterData>> m_map;
		static ParameterMap* s_instance;

		std::mutex m_paramMutex;

	};

}

#endif
