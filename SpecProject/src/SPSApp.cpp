/*
	main.cpp
	Entry point for the example SpecProject. Also contains example of a simple user Specter::Application.

	GWM -- Feb 2022
*/
#include "Specter.h"
#include "SPSAnalysisStage.h"
#include "SPSInputLayer.h"

//Including this inserts the predefined main function for Specter projects
//Can only be included once per project!
#include "Specter/Core/EntryPoint.h"

//User application class. Pushes user analysis stages.
class SPSApp : public Specter::Application
{
public:
	SPSApp(const Specter::ApplicationArgs& args) :
		Specter::Application(args)
	{
		PushLayer(new Specter::SPSInputLayer(m_manager));
		//PushLayer(new Navigator::TestServerLayer());
		PushAnalysisStage(new Specter::SPSAnalysisStage(m_manager));
	}
};

//Define the creation function to make our user application
Specter::Application* Specter::CreateApplication(const ApplicationArgs& args) { return new SPSApp(args); }