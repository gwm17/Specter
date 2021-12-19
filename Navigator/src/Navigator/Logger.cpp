#include <navpch.h>
#include "Logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Navigator {

	std::shared_ptr<spdlog::logger> Logger::s_logger;

	void Logger::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		s_logger = spdlog::stdout_color_mt("NAV");
		s_logger->set_level(spdlog::level::trace);
	}

}