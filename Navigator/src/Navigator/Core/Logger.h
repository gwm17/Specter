/*
	Logger.h
	Logging class which is a thin wrapper on the spdlog library. Must be initialized in the project at start (see NavProject main.cpp for example).
	Again, strongly based upon @TheCherno's work, see his Hazel repository for more details.

	Note Logger is a singleton. Should only ever be intialized once. Macros for calling the log provided to make clean looking code at the other side.
	Consider making some logging calls only defined on Debug.

	GWM -- Feb 2022
*/
#ifndef LOGGER_H
#define LOGGER_H

#include "NavCore.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Navigator {

	class NAV_API Logger
	{

	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger> GetLogger() { return s_logger; }

	private:
		static std::shared_ptr<spdlog::logger> s_logger;

	};

}

//Macros for clean code. Different logging levels.
#define NAV_CRITICAL(...) ::Navigator::Logger::GetLogger()->critical(__VA_ARGS__)
#define NAV_WARN(...) ::Navigator::Logger::GetLogger()->warn(__VA_ARGS__)
#define NAV_ERROR(...) ::Navigator::Logger::GetLogger()->error(__VA_ARGS__)
#define NAV_TRACE(...) ::Navigator::Logger::GetLogger()->trace(__VA_ARGS__)
#define NAV_INFO(...) ::Navigator::Logger::GetLogger()->info(__VA_ARGS__)

#endif