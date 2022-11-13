/*
	EntryPoint.h

	Predefined main function for Specter-based projects. Makes it so user doesn't have to know
	about initializing the Logger, Profiling sessions, etc.
*/
#ifndef ENTRY_POINT_H
#define ENTRY_POINT_H

#include "Specter/Core/Application.h"

//Make sure to initialize log BEFORE creating application.
int main(int argc, const char** argv)
{
	Specter::Logger::Init();
	SPEC_INFO("Logger Initialized");

	Specter::ApplicationArgs args;
	args.name = "Specter";
	args.runtimePath = std::filesystem::current_path();

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

#endif