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
#include "utils/path_utils.hpp"

#include <vector>
#include <ostream>

namespace data
{
	struct Settings
	{
		utf8_path explorer_folder;
		std::vector<utf8_path> music_sources;

		Settings() noexcept = default;
	};
	std::ostream& operator<<(std::ostream& os, const Settings& settings);

	bool saveSettings(Settings& settings,
	                  const std::shared_ptr<spdlog::logger>& logger = NULL_LOGGER) noexcept;

	Settings loadSettings(const std::shared_ptr<spdlog::logger>& logger = NULL_LOGGER) noexcept;

	bool equivalent_sources(const std::vector<utf8_path>& sources_a,
	                        const std::vector<utf8_path>& sources_b,
	                        const std::shared_ptr<spdlog::logger>& logger = NULL_LOGGER) noexcept;
} // namespace data

#endif //MAGICPLAYER_SETTINGS_HPP
