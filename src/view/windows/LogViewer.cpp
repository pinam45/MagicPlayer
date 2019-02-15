//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "view/windows/LogViewer.hpp"
#include "utils/log.hpp"

#include <spdlog/spdlog.h>

#include <cassert>
#include <utility>

namespace
{
	constexpr int DEFAULT_WIDTH = 900;
	constexpr int DEFAULT_HEIGHT = 450;

	constexpr std::array<float, 3> LOG_TRACE_COLOR{1.0f, 1.0f, 1.0f};
	constexpr std::array<float, 3> LOG_DEBUG_COLOR{0.557f, 0.808f, 1.0f};
	constexpr std::array<float, 3> LOG_INFO_COLOR{0.786f, 1.0f, 0.660f};
	constexpr std::array<float, 3> LOG_WARN_COLOR{1.0f, 0.969f, 0.118f};
	constexpr std::array<float, 3> LOG_ERR_COLOR{1.0f, 0.42f, 0.408f};
	constexpr std::array<float, 3> LOG_CRITICAL_COLOR{0.893f, 0.694f, 1.0f};
	constexpr std::array<float, 3> LOG_DEFAULT_COLOR{1.0f, 1.0f, 1.0f};
} // namespace

LogViewer::LogViewer(std::string name)
  : m_logs_colors()
  , m_name(std::move(name))
  , m_auto_scroll(true)
  , m_logger(spdlog::get(VIEW_LOGGER_NAME))
{
	assert(STORED_LOGS != nullptr);
	m_logs_colors[spdlog::level::trace] = {
	  LOG_TRACE_COLOR[0], LOG_TRACE_COLOR[1], LOG_TRACE_COLOR[2], 1.0};
	m_logs_colors[spdlog::level::debug] = {
	  LOG_DEBUG_COLOR[0], LOG_DEBUG_COLOR[1], LOG_DEBUG_COLOR[2], 1.0};
	m_logs_colors[spdlog::level::info] = {
	  LOG_INFO_COLOR[0], LOG_INFO_COLOR[1], LOG_INFO_COLOR[2], 1.0};
	m_logs_colors[spdlog::level::warn] = {
	  LOG_WARN_COLOR[0], LOG_WARN_COLOR[1], LOG_WARN_COLOR[2], 1.0};
	m_logs_colors[spdlog::level::err] = {LOG_ERR_COLOR[0], LOG_ERR_COLOR[1], LOG_ERR_COLOR[2], 1.0};
	m_logs_colors[spdlog::level::critical] = {
	  LOG_CRITICAL_COLOR[0], LOG_CRITICAL_COLOR[1], LOG_CRITICAL_COLOR[2], 1.0};
	m_logs_colors[spdlog::level::off] = {
	  LOG_DEFAULT_COLOR[0], LOG_DEFAULT_COLOR[1], LOG_DEFAULT_COLOR[2], 1.0};
}

void LogViewer::init()
{
}

void LogViewer::show(bool& open)
{
	ImGui::SetNextWindowSize(ImVec2(DEFAULT_WIDTH, DEFAULT_HEIGHT), ImGuiCond_Once);

	if(!ImGui::Begin(m_name.c_str(), &open))
	{
		ImGui::End();
		return;
	}

	ImGui::Checkbox("Scroll to bottom", &m_auto_scroll);
	if(ImGui::BeginChild("Logs", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
	{
		STORED_LOGS->iterate_on_logs([this](const store_sink_mt::store_log& log) {
			if(log.color_range_start < log.color_range_end)
			{
				ImGui::TextUnformatted(log.txt.c_str(), log.txt.c_str() + log.color_range_start);
				ImGui::SameLine(0, 0);
				ImGui::PushStyleColor(ImGuiCol_Text, m_logs_colors[log.level]);
				ImGui::TextUnformatted(log.txt.c_str() + log.color_range_start,
				                       log.txt.c_str() + log.color_range_end);
				ImGui::PopStyleColor();
				ImGui::SameLine(0, 0);
				ImGui::TextUnformatted(log.txt.c_str() + log.color_range_end,
				                       log.txt.c_str() + log.txt.size());
			}
			else
			{
				ImGui::TextUnformatted(log.txt.c_str());
			}
			return true;
		});
	}
	if(m_auto_scroll)
	{
		ImGui::SetScrollHereY(1.0f);
	}

	ImGui::EndChild();
	ImGui::End();
}
