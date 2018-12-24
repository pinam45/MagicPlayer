//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_AUDIO_EXTENSIONS_HPP
#define MAGICPLAYER_AUDIO_EXTENSIONS_HPP

#include "utils/path_utils.hpp"

#include <algorithm>
#include <array>
#include <filesystem>

constexpr std::array<std::string_view, 4> SUPPORTED_AUDIO_EXTENSIONS = {".vaw",
                                                                        ".ogg",
                                                                        ".flac",
                                                                        ".mp3"};

inline bool hasSupportedAudioExtension(const std::filesystem::path& path)
{
	return std::find(std::cbegin(SUPPORTED_AUDIO_EXTENSIONS),
	                 std::cend(SUPPORTED_AUDIO_EXTENSIONS),
	                 path_to_generic_utf8_string(path.extension()))
	       != std::cend(SUPPORTED_AUDIO_EXTENSIONS);
}

#endif //MAGICPLAYER_AUDIO_EXTENSIONS_HPP
