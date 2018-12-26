//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "utils/log.hpp"
#include "view/FontManager.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include <iostream>

namespace
{
	constexpr const char* LOG_FILENAME = "log.txt";
	constexpr std::size_t LOG_MAX_SIZE = 1024 * 1024 * 5;
	constexpr std::size_t LOG_MAX_FILES = 3;
} // namespace

bool init_logger()
{
	try
	{
		std::vector<spdlog::sink_ptr> sinks;

		std::shared_ptr<spdlog::sinks::sink> file_sink =
		  std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
		    LOG_FILENAME, LOG_MAX_SIZE, LOG_MAX_FILES);
		file_sink->set_level(spdlog::level::trace);
		sinks.push_back(file_sink);

#ifdef ENABLE_CONSOLE_LOG
		std::shared_ptr<spdlog::sinks::sink> console_sink =
		  std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		console_sink->set_level(spdlog::level::warn);
		sinks.push_back(console_sink);
#endif

		for(const char* logger_name: LOGGERS_NAMES)
		{
			std::shared_ptr<spdlog::logger> logger =
			  std::make_shared<spdlog::logger>(logger_name, sinks.cbegin(), sinks.cend());
			logger->set_level(spdlog::level::trace);
			logger->flush_on(spdlog::level::err);
			spdlog::register_logger(logger);
		}
		FontManager::setLogger(spdlog::get(VIEW_LOGGER_NAME));

		spdlog::get(GENERAL_LOGGER_NAME)->info("Logger initialised");
	}
	catch(const spdlog::spdlog_ex& ex)
	{
		std::cerr << "Logger initialization failed: " << ex.what() << std::endl;
		return false;
	}

	return true;
}
