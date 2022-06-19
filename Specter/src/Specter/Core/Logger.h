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

#include "SpecCore.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Specter {

	class Logger
	{

	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger> GetLogger() { return s_logger; }

	private:
		static std::shared_ptr<spdlog::logger> s_logger;

	};

}

//Macros for clean code. Different logging levels.
#define SPEC_CRITICAL(...) ::Specter::Logger::GetLogger()->critical(__VA_ARGS__)
#define SPEC_WARN(...) ::Specter::Logger::GetLogger()->warn(__VA_ARGS__)
#define SPEC_ERROR(...) ::Specter::Logger::GetLogger()->error(__VA_ARGS__)
#define SPEC_TRACE(...) ::Specter::Logger::GetLogger()->trace(__VA_ARGS__)
#define SPEC_INFO(...) ::Specter::Logger::GetLogger()->info(__VA_ARGS__)

#endif