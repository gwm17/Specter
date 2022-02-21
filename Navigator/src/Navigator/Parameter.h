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
        
        inline bool IsValid() const { return m_pdata->validFlag; }
        inline void Invalidate() { m_pdata->validFlag = false; }
        inline void SetValue(double value) { m_pdata->validFlag = true; m_pdata->value = value; }
        inline double GetValue() const { return m_pdata->value; }
        inline const std::string& GetName() const { return m_name; }

		friend class SpectrumManager;
	private:
		std::string m_name;
		std::shared_ptr<ParameterData> m_pdata;

	};

}

#endif
