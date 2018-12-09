//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_LOGVIEWER_HPP
#define MAGICPLAYER_LOGVIEWER_HPP

#include "utils/store_sink.hpp"

#include <spdlog/logger.h>
#include <imgui.h>

class LogViewer
{
public:
	explicit LogViewer(std::string name);

	LogViewer(const LogViewer&) = delete;
	LogViewer(LogViewer&&) = delete;
	LogViewer& operator=(const LogViewer&) = delete;
	LogViewer& operator=(LogViewer&&) = delete;

	~LogViewer();

	void watch_logger(const std::shared_ptr<spdlog::logger>& logger);

	void unwatch_logger(const std::shared_ptr<spdlog::logger>& logger);

	void init();

	void show(bool& open);

private:
	std::shared_ptr<store_sink_mt> m_logs_sink;
	std::vector<std::shared_ptr<spdlog::logger>> m_watched_loggers;
	std::unordered_map<spdlog::level::level_enum, ImVec4, spdlog::level::level_hasher>
	  m_logs_colors;

	std::string m_name;
	bool m_auto_scroll;

	std::shared_ptr<spdlog::logger> m_logger;
};

#endif //MAGICPLAYER_LOGVIEWER_HPP
