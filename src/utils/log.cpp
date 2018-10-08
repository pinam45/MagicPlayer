/*****************************************************************************************
 *                                                                                       *
 * MIT License                                                                           *
 *                                                                                       *
 * Copyright (c) 2018 Maxime Pinard                                                      *
 *                                                                                       *
 * Permission is hereby granted, free of charge, to any person obtaining a copy          *
 * of this software and associated documentation files (the "Software"), to deal         *
 * in the Software without restriction, including without limitation the rights          *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell             *
 * copies of the Software, and to permit persons to whom the Software is                 *
 * furnished to do so, subject to the following conditions:                              *
 *                                                                                       *
 * The above copyright notice and this permission notice shall be included in all        *
 * copies or substantial portions of the Software.                                       *
 *                                                                                       *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR            *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,              *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE           *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER                *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,         *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE         *
 * SOFTWARE.                                                                             *
 *                                                                                       *
 *****************************************************************************************/

#include "utils/log.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include <iostream>

constexpr const char* LOG_FILENAME = "log.txt";
constexpr std::size_t LOG_MAX_SIZE = 1024 * 1024 * 5;
constexpr std::size_t LOG_MAX_FILES = 3;

bool init_logger() {
	try{
		std::vector<spdlog::sink_ptr> sinks;

		std::shared_ptr<spdlog::sinks::sink> file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(LOG_FILENAME, LOG_MAX_SIZE, LOG_MAX_FILES);
		file_sink->set_level(spdlog::level::trace);
		sinks.push_back(file_sink);

#ifdef ENABLE_CONSOLE_LOG
		std::shared_ptr<spdlog::sinks::sink> console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		console_sink->set_level(spdlog::level::warn);
		sinks.push_back(console_sink);
#endif

		for(const char* logger_name : LOGGERS_NAMES){
			std::shared_ptr<spdlog::logger> logger = std::make_shared<spdlog::logger>(logger_name, sinks.cbegin(), sinks.cend());
			logger->set_level(spdlog::level::trace);
			logger->flush_on(spdlog::level::err);
			spdlog::register_logger(logger);
		}

		spdlog::get(GENERAL_LOGGER_NAME)->info("Logger initialised");
	}
	catch (const spdlog::spdlog_ex& ex)
	{
		std::cerr << "Logger initialization failed: " << ex.what() << std::endl;
		return false;
	}

	return true;
}
