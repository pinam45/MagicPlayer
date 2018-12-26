//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "view/FontManager.hpp"

#include <IconsFontAwesome5.h>
#include <imgui.h>
#include <imgui-SFML.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/null_sink.h>

#include <mutex>
#include <unordered_map>

namespace
{
	constexpr const char* DROID_SANS_MONO_FONT_PATH = "resources/fonts/DroidSans/DroidSansMono.ttf";
	constexpr const char* FONTAWESOME_FONT_PATH =
	  "resources/fonts/fontawesome-free-5.4.0/" FONT_ICON_FILE_NAME_FAS;
	constexpr const char* DEFAULT_FONT_PATH = DROID_SANS_MONO_FONT_PATH;

	std::shared_ptr<spdlog::logger> NULL_LOGGER =
	  std::make_shared<spdlog::logger>("null", std::make_shared<spdlog::sinks::null_sink_st>());

	struct FontManagerData
	{
		std::mutex m_mutex;
		std::unordered_map<float, ImFont*> fonts;

		std::shared_ptr<spdlog::logger> logger = NULL_LOGGER;
	};
	FontManagerData DATA;

	// Warning: not thread safe, doesn't lock m_mutex
	ImFont* loadFont(float size)
	{
		ImGuiIO& io = ImGui::GetIO();

		static constexpr ImWchar font_ranges[] = {0x0001, 0xFFFF, 0};
		ImFont* default_font =
		  io.Fonts->AddFontFromFileTTF(DEFAULT_FONT_PATH, size, nullptr, font_ranges);
		if(default_font)
		{
			SPDLOG_DEBUG(DATA.logger, "Loaded font {} {:.2f}px", DEFAULT_FONT_PATH, size);
		}
		else
		{
			io.Fonts->AddFontDefault();
			DATA.logger->warn("Failed to load font {}: use default font instead",
			                  DEFAULT_FONT_PATH);
		}

		static constexpr ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
		ImFontConfig icons_config;
		icons_config.MergeMode = true;
		icons_config.PixelSnapH = true;
		icons_config.GlyphMinAdvanceX = size;
		ImFont* font =
		  io.Fonts->AddFontFromFileTTF(FONTAWESOME_FONT_PATH, size, &icons_config, icons_ranges);
		if(font)
		{
			SPDLOG_DEBUG(DATA.logger, "Loaded font {} {:.2f}px", FONTAWESOME_FONT_PATH, size);
		}
		else
		{
			DATA.logger->warn("Failed to load fontawesome ({}): icons disabled",
			                  FONTAWESOME_FONT_PATH);
			font = default_font;
		}

		return font;
	}
} // namespace

void FontManager::loadFontSize(float size)
{
	std::lock_guard guard(DATA.m_mutex);
	ImFont*& font = DATA.fonts[size];
	if(font != nullptr)
	{
		return;
	}
	font = loadFont(size);
	ImGui::SFML::UpdateFontTexture();
}

void FontManager::pushFontSize(float size)
{
	std::lock_guard guard(DATA.m_mutex);
	ImFont*& font = DATA.fonts[size];
	if(font == nullptr)
	{
		font = loadFont(size);
		ImGui::SFML::UpdateFontTexture();
	}
	ImGui::PushFont(font);
}

void FontManager::popFontSize(int count)
{
	std::lock_guard guard(DATA.m_mutex);
	for(int i = 0; i < count; ++i)
	{
		ImGui::PopFont();
	}
}

void FontManager::setLogger(std::shared_ptr<spdlog::logger> logger)
{
	if(logger)
	{
		DATA.logger = std::move(logger);
	}
	else
	{
		DATA.logger = NULL_LOGGER;
	}
}
