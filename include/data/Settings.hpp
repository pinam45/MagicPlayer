//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_SETTINGS_HPP
#define MAGICPLAYER_SETTINGS_HPP

#include "utils/log.hpp"

#include <vector>
#include <filesystem>
#include <ostream>

namespace data
{
	struct Settings
	{
		std::filesystem::path explorer_folder;
		std::vector<std::filesystem::path> music_sources;
	};
	std::ostream& operator<<(std::ostream& os, const Settings& settings);

	bool saveSettings(const Settings& settings,
	                  const std::shared_ptr<spdlog::logger>& logger = NULL_LOGGER) noexcept;

	Settings loadSettings(const std::shared_ptr<spdlog::logger>& logger = NULL_LOGGER) noexcept;
} // namespace data

#endif //MAGICPLAYER_SETTINGS_HPP
