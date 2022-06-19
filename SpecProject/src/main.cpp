/*
	main.cpp
	Entry point for the example NavProject. Also contains example of a simple user Navigator::Application.

	GWM -- Feb 2022
*/
#include "Specter.h"
#include "SPSAnalysisStage.h"
#include "SPSInputLayer.h"

//User application class. Pushes user analysis stages.
class SPSApp : public Specter::Application
{
public:
	SPSApp() :
		Specter::Application()
	{
		PushLayer(new Specter::SPSInputLayer());
		//PushLayer(new Navigator::TestServerLayer());
		PushAnalysisStage(new Specter::SPSAnalysisStage());
	}
};

//Define the creation function to make our user application
Specter::Application* Specter::CreateApplication() { return new SPSApp(); }

//Make sure to initialize log BEFORE creating application.
int main(int argc, const char** argv)
{
	Specter::Logger::Init();
	SPEC_TRACE("Logger Initialized!");

	SPEC_PROFILE_BEGIN_SESSION("Startup", "navprofile_startup.json");
	auto app = Specter::CreateApplication();
	SPEC_PROFILE_END_SESSION();

	SPEC_PROFILE_BEGIN_SESSION("Runtime", "navprofile_runtime.json");
	app->Run();
	SPEC_PROFILE_END_SESSION();

	SPEC_PROFILE_BEGIN_SESSION("Shutdown", "navprofile_shutdown.json");
	delete app;
	SPEC_PROFILE_END_SESSION();
}