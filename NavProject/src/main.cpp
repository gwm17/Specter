/*
	main.cpp
	Entry point for the example NavProject. Also contains example of a simple user Navigator::Application.

	GWM -- Feb 2022
*/
#include "Navigator.h"
#include "SPSAnalysisStage.h"
#include "SPSInputLayer.h"

//User application class. Pushes user analysis stages.
class SPSApp : public Navigator::Application
{
public:
	SPSApp() :
		Navigator::Application()
	{
		PushLayer(new Navigator::SPSInputLayer());
		//PushLayer(new Navigator::TestServerLayer());
		PushAnalysisStage(new Navigator::SPSAnalysisStage());
	}
};

//Define the creation function to make our user application
Navigator::Application* Navigator::CreateApplication() { return new SPSApp(); }

//Make sure to initialize log BEFORE creating application.
int main(int argc, const char** argv)
{
	Navigator::Logger::Init();
	NAV_TRACE("Logger Initialized!");

	auto app = Navigator::CreateApplication();

	app->Run();

	delete app;
}