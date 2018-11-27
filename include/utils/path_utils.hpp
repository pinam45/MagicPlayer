//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_PATH_UTILS_HPP
#define MAGICPLAYER_PATH_UTILS_HPP

#include <string>
#include <filesystem>

// Convert an utf8 string to a path.
// return false on error, true on success
[[nodiscard]] bool utf8_string_to_path(const std::string& str, std::filesystem::path& path) noexcept;

// Convert a path to a generic utf8 string.
// return false on error, true on success
[[nodiscard]] bool path_to_generic_utf8_string(const std::filesystem::path& path, std::string& str) noexcept;

// Generate a debug representation of an invalid utf8 path.
// use to log when path_to_generic_utf8_string() fail (return false) to get a string with
// the path until the invalid utf8 character followed by "(?) [invalid utf8]"
std::string invalid_utf8_path_representation(const std::filesystem::path& path) noexcept;

// Convert an utf8 string to a path.
// stop conversion on error: use only for logging
std::filesystem::path utf8_string_to_path(const std::string& str) noexcept;

// Convert a path to a generic utf8 string.
// stop conversion on error: use only for logging
std::string path_to_generic_utf8_string(std::filesystem::path path) noexcept;

#endif //MAGICPLAYER_PATH_UTILS_HPP
