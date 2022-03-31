/*
	Parameter.cpp
	Contains two data related structures, ParameterData and NavParameter. Parameter here refers to a value which
	can be associated with the axis of a histogram. ParameterData is a two component struct which represents the state of a single parameter, which is stored
	in the global SpectrumManager (see SpectrumManager documentation). NavParameter is an access point class for use within analysis. It provides scoped access to
	the managed data. There are several important caveats to using the parameters this way, which is mostly related to synchronization:

	- NavParameter contains no guarantee of thread safety. THIS IS BY DESIGN. NavParameters should only be created within the context of an AnalysisStage (see AnalysisStage documentation).
	  As long as this is true, there is no risk of accessing parameter data outside of the physics thread, and that thread explicitly handles calculation of parameters
	  and updating of histograms. DO NOT make a NavParameter outside of the AnalysisStage context.
	- NavParameters must be bound to the active SpectrumManager. This is done using the BindParameter function of the manager; NavParameters are keyed based on their name string.
	  If two NavParameters are made that are both named "x1", they are in fact the same parameter. In this way, values can be passed from one AnalysisStage to another. If you make a stage
	  called InitialStage with a NavParameter named "x1" where x1 is set to 1.0, and then have a later stage called LaterStage with another NavParameter named x1, it implicitly has the value 1.0
	  due to it being set in the previous stage.
	- Each NavParameter has a valid flag. This is a boolean which idicates whether or not the parameter is in a valid state (the data event contianed a value  for this parameter). Before using a parameter
	  in a calculation one should check if the parameter is valid using the IsValid() function. When a parameter is set using the SetValue() function, the valid flag is set to true. After the event is completely
	  processed (all analysis stages have been run and histograms have been updated) the manager should be called to run InvalidateParameters() to set all parameters as invalid (valid flag false).

	Those are the key points I can think of now. As more people use the code, I expect this section to grow and change.

	Credit to nscldaq and in particular NSCLSpecTcl which provided the inspiration for this parameter model.

	GWM -- Feb 2022
*/
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

	//So that you can make arrays, std::vectors of NavParameters (useful for big detector arrays)
	//SpectrumManager::BindParameter() still needs to be used after this!
	void NavParameter::SetName(const std::string& name)
	{
		if (m_name != "")
		{
			NAV_ERROR("Attempting to change the name of an already bound NavParameter! Set name: {0} New name: {1}", m_name, name);
			return;
		}
		m_name = name;
	}

	NavVariable::NavVariable() :
		m_name(""), m_pdata(nullptr)
	{
	}

	NavVariable::NavVariable(const std::string& name) :
		m_name(name), m_pdata(nullptr)
	{
	}

	NavVariable::~NavVariable() {}
}
