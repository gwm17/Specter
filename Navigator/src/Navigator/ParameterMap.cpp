//
//  ParameterMap.cpp
//  Navigator
//
//  Created by Gordon McCann on 1/7/22.
//

#include "ParameterMap.h"

namespace Navigator {

    NavParameter::NavParameter(const std::string& name, const std::string& alias) :
        m_name(name)
    {
        NAV_INFO("Making a new parameter named {0}...",name);
        m_pdata = nullptr;
        ParameterMap& map = ParameterMap::GetInstance();
        auto iter = map.find(name);
        if(iter == map.end())
        {
            NAV_INFO("Added it to the map.");
            map.AddParameter(name);
        }
        
        NAV_INFO("Setting the memory...");
        map.SetParameter(name, m_pdata);
    }

    NavParameter::~NavParameter() {}

    ParameterMap* ParameterMap::s_instance = nullptr;

    ParameterMap* CreateParameterMap() { return new ParameterMap(); }

    ParameterMap::ParameterMap()
    {
        NAV_INFO("Created ParameterMap");
        if(s_instance != nullptr)
            NAV_ERROR("Cannot have multiple instances of ParameterMap!!!");
        
        s_instance = this;
    }

    ParameterMap::~ParameterMap() {}

    double ParameterMap::GetParameterValue(const std::string& name)
    {
        auto iter = m_map.find(name);
        if(iter != m_map.end())
            return iter->second->value;
        else
            return 0.0;
    }

    bool ParameterMap::IsParameterValid(const std::string& name)
    {
        auto iter = m_map.find(name);
        if(iter != m_map.end())
            return iter->second->validFlag;
        else
            return false;
    }

    void ParameterMap::InvalidateParameters()
    {
        for(auto& iter : m_map)
        {
            iter.second->validFlag = false;
            iter.second->value = 0.0;
        }
    }

    std::vector<std::string> ParameterMap::GetListOfParameters()
    {
        std::vector<std::string> list;
        list.reserve(m_map.size());
        for (auto iter : m_map)
        {
            list.push_back(iter.first);
        }

        return list;
    }
}
