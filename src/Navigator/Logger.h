#ifndef LOGGER_H
#define LOGGER_H

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Navigator {

	class Logger
	{

	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger> GetLogger() { return s_logger; }

	private:
		static std::shared_ptr<spdlog::logger> s_logger;

	};

}

#define NAV_CRITICAL(...) ::Navigator::Logger::GetLogger()->critical(__VA_ARGS__)
#define NAV_WARN(...) ::Navigator::Logger::GetLogger()->warn(__VA_ARGS__)
#define NAV_ERROR(...) ::Navigator::Logger::GetLogger()->error(__VA_ARGS__)
#define NAV_TRACE(...) ::Navigator::Logger::GetLogger()->trace(__VA_ARGS__)
#define NAV_INFO(...) ::Navigator::Logger::GetLogger()->info(__VA_ARGS__)

#endif