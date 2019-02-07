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

class utf8_path final
{
public:
	utf8_path() noexcept;
	~utf8_path() noexcept = default;

	utf8_path(const utf8_path& other) noexcept;
	utf8_path(utf8_path&& other) noexcept;
	utf8_path(const char* str) noexcept;
	utf8_path(std::string_view str) noexcept;
	utf8_path(std::string&& str) noexcept;
	utf8_path(const std::filesystem::path& path) noexcept;
	utf8_path(std::filesystem::path&& path) noexcept;

	utf8_path& operator=(const utf8_path& other) noexcept;
	utf8_path& operator=(utf8_path&& other) noexcept;
	utf8_path& operator=(const char* str) noexcept;
	utf8_path& operator=(std::string_view str) noexcept;
	utf8_path& operator=(std::string&& str) noexcept;
	utf8_path& operator=(const std::filesystem::path& path) noexcept;
	utf8_path& operator=(std::filesystem::path&& path) noexcept;

	std::string_view str() const noexcept;
	const std::string& str_cref() const noexcept;
	const std::filesystem::path& path() const noexcept;
	bool valid() const noexcept;

private:
	void set_str() noexcept;
	void set_path() noexcept;

	bool m_valid;
	std::string m_utf8_str;
	std::filesystem::path m_path;
};
std::ostream& operator<<(std::ostream& os, const utf8_path& utf8_path);

// Convert an utf8 string to a path.
// return false on error, true on success
[[nodiscard]] bool utf8_string_to_path(std::string_view str, std::filesystem::path& path) noexcept;

// Convert a path to a generic utf8 string.
// return false on error, true on success
[[nodiscard]] bool path_to_generic_utf8_string(const std::filesystem::path& path,
                                               std::string& str) noexcept;

// Generate a debug representation of an invalid utf8 path.
// use to log when path_to_generic_utf8_string() fail (return false) to get a string with
// the path until the invalid utf8 character followed by "(?) [invalid utf8]"
std::string invalid_utf8_path_representation(const std::filesystem::path& path) noexcept;

// Convert an utf8 string to a path.
// stop conversion on error: use only for logging
std::filesystem::path utf8_string_to_path(std::string_view str) noexcept;

// Convert a path to a generic utf8 string.
// stop conversion on error: use only for logging
std::string path_to_generic_utf8_string(std::filesystem::path path) noexcept;

#endif //MAGICPLAYER_PATH_UTILS_HPP
