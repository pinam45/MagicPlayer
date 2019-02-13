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
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <taglib/tdebuglistener.h>

#include <cassert>
#include <iostream>

const std::shared_ptr<spdlog::logger> NULL_LOGGER =
  std::make_shared<spdlog::logger>("null", std::make_shared<spdlog::sinks::null_sink_mt>());

const std::shared_ptr<store_sink_mt> STORED_LOGS = std::make_shared<store_sink_mt>();

namespace
{
	constexpr const char* LOG_FILENAME = "log.txt";
	constexpr std::size_t LOG_MAX_SIZE = 1024 * 1024 * 5;
	constexpr std::size_t LOG_MAX_FILES = 3;

	struct TagLibLogger final : public TagLib::DebugListener
	{
		void printMessage(const TagLib::String& msg) override;

		std::shared_ptr<spdlog::logger> logger = NULL_LOGGER;
	};

	void TagLibLogger::printMessage(const TagLib::String& msg)
	{
		std::string str(msg.to8Bit(true));
		std::string_view view(str);
		view.remove_suffix(1); // remove trailing '\n'
		SPDLOG_DEBUG(logger, "{}", view);
	}
	TagLibLogger TAGLIB_LOGGER;
} // namespace

bool init_logger()
{
	assert(NULL_LOGGER != nullptr);
	try
	{
		// Configure sinks
		std::vector<spdlog::sink_ptr> sinks;

		// Store sink
		assert(STORED_LOGS != nullptr);
		STORED_LOGS->set_level(spdlog::level::trace);
		sinks.push_back(STORED_LOGS);

		// File sink
		std::shared_ptr<spdlog::sinks::sink> file_sink =
		  std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
		    LOG_FILENAME, LOG_MAX_SIZE, LOG_MAX_FILES);
		assert(file_sink != nullptr);
		file_sink->set_level(spdlog::level::trace);
		sinks.push_back(file_sink);

		// Console sink
#ifdef ENABLE_CONSOLE_LOG
		std::shared_ptr<spdlog::sinks::sink> console_sink =
		  std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		assert(console_sink != nullptr);
		console_sink->set_level(spdlog::level::warn);
		sinks.push_back(console_sink);
#endif

		// Configure loggers
		for(const char* logger_name: LOGGERS_NAMES)
		{
			std::shared_ptr<spdlog::logger> logger =
			  std::make_shared<spdlog::logger>(logger_name, sinks.cbegin(), sinks.cend());
			assert(logger != nullptr);
			logger->set_level(spdlog::level::trace);
			logger->flush_on(spdlog::level::err);
			spdlog::register_logger(logger);
		}

		// Set FontManager logger
		FontManager::setLogger(spdlog::get(VIEW_LOGGER_NAME));

		// Set TagLib logger
		TAGLIB_LOGGER.logger = spdlog::get(LOGIC_LOGGER_NAME);
		TagLib::setDebugListener(&TAGLIB_LOGGER);

		spdlog::get(GENERAL_LOGGER_NAME)->info("Loggers initialised");
	}
	catch(const spdlog::spdlog_ex& ex)
	{
		std::cerr << "Logger initialization failed: " << ex.what() << std::endl;
		return false;
	}

	return true;
}
