#include "Navigator.h"

Navigator::Application* Navigator::CreateApplication() { return new Application(); }
Navigator::PhysicsEventBuilder* Navigator::CreatePhysicsEventBuilder() { return new PhysicsEventBuilder(); }

int main(int argc, const char** argv)
{
	Navigator::Logger::Init();
	NAV_TRACE("Logger Initialized!");

	auto app = Navigator::CreateApplication();

	app->Run();

	delete app;
}