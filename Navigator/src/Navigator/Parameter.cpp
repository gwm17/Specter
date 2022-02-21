#include "Parameter.h"

namespace Navigator {

    NavParameter::NavParameter() :
        m_name(""), m_pdata(nullptr)
    {
    }

    NavParameter::NavParameter(const std::string& name) :
        m_name(name), m_pdata(nullptr)
    {
        NAV_INFO("Making a new parameter named {0}...",name);
    }

    NavParameter::~NavParameter() {}

}
