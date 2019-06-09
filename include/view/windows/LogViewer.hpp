//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_LOGVIEWER_HPP
#define MAGICPLAYER_LOGVIEWER_HPP

#include <spdlog/logger.h>
#include <imgui.h>

class LogViewer
{
public:
	explicit LogViewer(std::string name);

	LogViewer(const LogViewer&) = default;
	LogViewer(LogViewer&&) noexcept = default;
	LogViewer& operator=(const LogViewer&) = default;
	LogViewer& operator=(LogViewer&&) noexcept = default;

	~LogViewer() noexcept = default;

	void init();

	void show(bool& open);

private:
	std::unordered_map<spdlog::level::level_enum, ImVec4, spdlog::level::level_hasher>
	  m_logs_colors;
	spdlog::level::level_enum m_log_level;

	std::string m_name;
	bool m_auto_scroll;
	bool m_wrap_lines;

	std::shared_ptr<spdlog::logger> m_logger;
};

#endif //MAGICPLAYER_LOGVIEWER_HPP
