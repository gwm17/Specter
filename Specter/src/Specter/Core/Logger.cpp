/*
	Logger.cpp
	Logging class which is a thin wrapper on the spdlog library. Must be initialized in the project at start (see NavProject main.cpp for example).
	Again, strongly based upon @TheCherno's work, see his Hazel repository for more details.

	Note Logger is a singleton. Should only ever be intialized once. Macros for calling the log provided to make clean looking code at the other side.
	Consider making some logging calls only defined on Debug.

	GWM -- Feb 2022
*/
#include "Logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Specter {

	std::shared_ptr<spdlog::logger> Logger::s_logger;

	void Logger::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		s_logger = spdlog::stdout_color_mt("SPEC");
		s_logger->set_level(spdlog::level::trace);
	}

}