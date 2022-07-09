/*
	main.cpp
	Entry point for the example NavProject. Also contains example of a simple user Navigator::Application.

	GWM -- Feb 2022
*/
#include "Specter.h"
#include "SPSAnalysisStage.h"
#include "SPSInputLayer.h"
#include <filesystem>

//User application class. Pushes user analysis stages.
class SPSApp : public Specter::Application
{
public:
	SPSApp(const Specter::ApplicationArgs& args) :
		Specter::Application(args)
	{
		PushLayer(new Specter::SPSInputLayer());
		//PushLayer(new Navigator::TestServerLayer());
		PushAnalysisStage(new Specter::SPSAnalysisStage());
	}
};

//Define the creation function to make our user application
Specter::Application* Specter::CreateApplication(const ApplicationArgs& args) { return new SPSApp(args); }

//Make sure to initialize log BEFORE creating application.
int main(int argc, const char** argv)
{
	Specter::Logger::Init();
	SPEC_TRACE("Logger Initialized!");

	Specter::ApplicationArgs args;
	args.name = "SPS Specter";
	if (std::filesystem::current_path().string().find("SpecProject") != std::string::npos)
		args.runtimePath = ""; //Dont modify runtime path, already points to SpecProject
	else
		args.runtimePath = "../SpecProject"; //Assume we're attempting to run from bin dir? Technically would also work for any new subproject made by same method as SpecProject

	SPEC_PROFILE_BEGIN_SESSION("Startup", "navprofile_startup.json");
	auto app = Specter::CreateApplication(args);
	SPEC_PROFILE_END_SESSION();

	SPEC_PROFILE_BEGIN_SESSION("Runtime", "navprofile_runtime.json");
	app->Run();
	SPEC_PROFILE_END_SESSION();

	SPEC_PROFILE_BEGIN_SESSION("Shutdown", "navprofile_shutdown.json");
	delete app;
	SPEC_PROFILE_END_SESSION();
}