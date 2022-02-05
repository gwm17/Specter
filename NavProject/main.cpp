#include "Navigator.h"
#include "SPSAnalysisStage.h"

class SPSApp : public Navigator::Application
{
public:
	SPSApp() :
		Navigator::Application()
	{
		PushAnalysisStage(new Navigator::SPSAnalysisStage());
	}
};

Navigator::Application* Navigator::CreateApplication() { return new SPSApp(); }

int main(int argc, const char** argv)
{
	Navigator::Logger::Init();
	NAV_TRACE("Logger Initialized!");

	auto app = Navigator::CreateApplication();

	app->Run();

	delete app;
}