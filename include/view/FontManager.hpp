//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_FONTMANAGER_HPP
#define MAGICPLAYER_FONTMANAGER_HPP

#include <spdlog/spdlog.h>

struct FontManager
{
	static constexpr float DEFAULT_FONT_SIZE = 13.5f;
	static constexpr float LARGE_FONT_SIZE = 22.0f;

	static void loadFontSize(float size);

	static void pushFontSize(float size);
	static void popFontSize(int count = 1);

	static void setLogger(std::shared_ptr<spdlog::logger> logger);
};

#endif //MAGICPLAYER_FONTMANAGER_HPP
