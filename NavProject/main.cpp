#include "Navigator.h"

Navigator::Application* Navigator::CreateApplication() { return new Application(); }

int main(int argc, const char** argv)
{
	Navigator::Logger::Init();
	NAV_TRACE("Logger Initialized!");

	auto app = Navigator::CreateApplication();

	NAV_TRACE("Navigator Application Created!");
	app->Run();

	delete app;
}