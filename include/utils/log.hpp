//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_LOG_HPP
#define MAGICPLAYER_LOG_HPP

#include "utils/path_utils.hpp"
#include "utils/store_sink.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/fmt/ostr.h>

constexpr const char* GENERAL_LOGGER_NAME = "general";
constexpr const char* LOGIC_LOGGER_NAME = "logic";
constexpr const char* VIEW_LOGGER_NAME = "view";

constexpr const char* LOGGERS_NAMES[] = {GENERAL_LOGGER_NAME, LOGIC_LOGGER_NAME, VIEW_LOGGER_NAME};

extern const std::shared_ptr<spdlog::logger> NULL_LOGGER;

extern const std::shared_ptr<store_sink_mt> STORED_LOGS;

inline std::ostream& operator<<(std::ostream& os, const std::filesystem::path& p)
{
	return os << '\"' << path_to_generic_utf8_string(p) << '\"';
}

bool init_logger();

#endif //MAGICPLAYER_LOG_HPP
