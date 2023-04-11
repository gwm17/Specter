/*
	Parameter.cpp
	Contains two data related structures, ParameterData and Parameter. Parameter here refers to a value which
	can be associated with the axis of a histogram. ParameterData is a two component struct which represents the state of a single parameter, which is stored
	in the global SpectrumManager (see SpectrumManager documentation). Parameter is an access point class for use within analysis. It provides scoped access to
	the managed data. There are several important caveats to using the parameters this way, which is mostly related to synchronization:

	- Parameter contains no guarantee of thread safety. THIS IS BY DESIGN. Parameters should only be created within the context of an AnalysisStage (see AnalysisStage documentation).
	  As long as this is true, there is no risk of accessing parameter data outside of the physics thread, and that thread explicitly handles calculation of parameters
	  and updating of histograms. DO NOT make a Parameter outside of the AnalysisStage context.
	- Parameters must be bound to the active SpectrumManager. This is done using the BindParameter function of the manager; Parameters are keyed based on their name string.
	  If two Parameters are made that are both named "x1", they are in fact the same parameter. In this way, values can be passed from one AnalysisStage to another. If you make a stage
	  called InitialStage with a Parameter named "x1" where x1 is set to 1.0, and then have a later stage called LaterStage with another Parameter named x1, it implicitly has the value 1.0
	  due to it being set in the previous stage.
	- Each Parameter has a valid flag. This is a boolean which idicates whether or not the parameter is in a valid state (the data event contianed a value  for this parameter). Before using a parameter
	  in a calculation one should check if the parameter is valid using the IsValid() function. When a parameter is set using the SetValue() function, the valid flag is set to true. After the event is completely
	  processed (all analysis stages have been run and histograms have been updated) the manager should be called to run InvalidateParameters() to set all parameters as invalid (valid flag false).

	Those are the key points I can think of now. As more people use the code, I expect this section to grow and change.

	Credit to nscldaq and in particular NSCLSpecTcl which provided the inspiration for this parameter model.

	GWM -- Feb 2022

	Variables added; similar to Parameters, but intend to be an interface with UI feedback. See SpecProject for examples. -- GWM April 2023

	Scalers added. In nuclear phyiscs, scalers refer to time counters of data, to track the rate of different detector components. -- GWM April 2023
*/
#include "Parameter.h"

namespace Specter {

    Parameter::Parameter() :
        m_name(""), m_pdata(nullptr)
    {
    }

    Parameter::Parameter(const std::string& name) :
        m_name(name), m_pdata(nullptr)
    {
    }

    Parameter::~Parameter() {}

	//So that you can make arrays, std::vectors of Parameters (useful for big detector arrays)
	//SpectrumManager::BindParameter() still needs to be used after this!
	void Parameter::SetName(const std::string& name)
	{
		if (m_name != "")
		{
			SPEC_ERROR("Attempting to change the name of an already bound Parameter! Set name: {0} New name: {1}", m_name, name);
			return;
		}
		m_name = name;
	}

	Variable::Variable() :
		m_name(""), m_pdata(nullptr)
	{
	}

	Variable::Variable(const std::string& name) :
		m_name(name), m_pdata(nullptr)
	{
	}

	Variable::~Variable() {}

	void Variable::SetName(const std::string& name)
	{
		if (m_name != "")
		{
			SPEC_ERROR("Attempting to change the name of an already bound Variable! Set name: {0} New name: {1}", m_name, name);
			return;
		}
		m_name = name;
	}

	Scaler::Scaler() :
		m_pdata(nullptr), m_name("")
	{
	}

	Scaler::Scaler(const std::string& name) :
		m_pdata(nullptr), m_name(name)
	{
	}

	Scaler::~Scaler() {}

	void Scaler::SetName(const std::string& name)
	{
		if (m_name != "")
		{
			SPEC_ERROR("Attempting to change the name of an already bound Scaler! Set name: {0} New name: {1}", m_name, name);
			return;
		}
		m_name = name;
	}
}
