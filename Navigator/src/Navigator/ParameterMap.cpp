//
//  ParameterMap.cpp
//  Navigator
//
//  Created by Gordon McCann on 1/7/22.
//

#include "ParameterMap.h"

namespace Navigator {

    NavParameter::NavParameter() :
        m_name(""), m_pdata(nullptr)
    {
    }

    NavParameter::NavParameter(const std::string& name) :
        m_name(name)
    {
        NAV_INFO("Making a new parameter named {0}...",name);
        m_pdata = nullptr;
        ParameterMap& map = ParameterMap::GetInstance();
        NAV_INFO("Setting the memory...");
        map.SetParameter(name, m_pdata);
    }

    NavParameter::~NavParameter() {}

    void NavParameter::SetParameter(const std::string& name)
    {
        ParameterMap& map = ParameterMap::GetInstance();
        map.SetParameter(name, m_pdata);
    }

    ParameterMap* ParameterMap::s_instance = new ParameterMap();

    ParameterMap::ParameterMap()
    {
    }

    ParameterMap::~ParameterMap() {}

    void ParameterMap::SetParameter(const std::string& name, std::shared_ptr<ParameterData>& param)
    {
        std::lock_guard<std::mutex> guard(m_paramMutex);
        auto iter = m_map.find(name);
        if(iter == m_map.end())
        {
            m_map[name].reset(new ParameterData());
        }

        param = m_map[name];
    }

    ParameterData ParameterMap::GetParameter(const std::string& name)
    {
        std::lock_guard<std::mutex> guard(m_paramMutex);
        auto iter = m_map.find(name);
        if(iter != m_map.end())
            return *(iter->second);
        else
            return ParameterData();
    }

    bool ParameterMap::IsParameterValid(const std::string& name)
    {
        std::lock_guard<std::mutex> guard(m_paramMutex);
        auto iter = m_map.find(name);
        if(iter != m_map.end())
            return iter->second->validFlag;
        else
            return false;
    }

    void ParameterMap::InvalidateParameters()
    {
        std::lock_guard<std::mutex> guard(m_paramMutex);
        for(auto& iter : m_map)
        {
            iter.second->validFlag = false;
            iter.second->value = 0.0;
        }
    }

    std::vector<std::string> ParameterMap::GetListOfParameters()
    {
        std::lock_guard<std::mutex> guard(m_paramMutex);
        std::vector<std::string> list;
        list.reserve(m_map.size());
        for (auto iter : m_map)
        {
            list.push_back(iter.first);
        }

        return list;
    }
}
