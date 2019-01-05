//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "data/Settings.hpp"
#include "utils/log.hpp"
#include "utils/path_utils.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <iomanip>

namespace
{
	constexpr const char* DEFAULT_EXPLORER_FOLDER = "./";
	constexpr const char* SETTINGS_FILE_PATH = "MagicPlayer_settings.json";
} // namespace

std::ostream& data::operator<<(std::ostream& os, const Settings& settings)
{
	os << "Settings{"
	   << "explorer_folder: " << settings.explorer_folder << ","
	   << "music_sources: [";
	for(const auto& source: settings.music_sources)
	{
		os << source << ",";
	}
	os << "]}";
	return os;
}

bool data::saveSettings(const data::Settings& settings,
                        const std::shared_ptr<spdlog::logger>& logger) noexcept
{
	nlohmann::json settings_json;
	std::error_code error;
	std::filesystem::path path = std::filesystem::canonical(settings.explorer_folder, error);
	if(error)
	{
		SPDLOG_DEBUG(logger, "std::filesystem::canonical failed: {}", error.message());
		logger->warn("Failed to get canonical path of explorer folder {}, path will be saved as is",
		             settings.explorer_folder);
		path = settings.explorer_folder;
	}
	std::string explorer_path_str;
	if(!path_to_generic_utf8_string(path, explorer_path_str))
	{
		logger->warn("Explorer folder have an invalid utf8 path and can't be saved: {}",
		             invalid_utf8_path_representation(path));
	}
	else
	{
		settings_json["explorer_folder"] = std::move(explorer_path_str);
	}

	std::vector<std::string> music_sources_str;
	music_sources_str.reserve(settings.music_sources.size());
	for(const std::filesystem::path& music_source: settings.music_sources)
	{
		path = std::filesystem::canonical(music_source, error);
		if(error)
		{
			SPDLOG_DEBUG(logger, "std::filesystem::canonical failed: {}", error.message());
			logger->warn(
			  "Failed to get canonical path of music source {}, path will be saved as is",
			  music_source);
			path = music_source;
		}
		std::string path_str;
		if(!path_to_generic_utf8_string(path, path_str))
		{
			logger->warn("Music source have an invalid utf8 path and can't be saved: {}",
			             invalid_utf8_path_representation(path));
		}
		else
		{
			music_sources_str.push_back(std::move(path_str));
		}
	}
	settings_json["music_sources"] = std::move(music_sources_str);

	std::ofstream file_stream(SETTINGS_FILE_PATH);
	if(!file_stream)
	{
		SPDLOG_DEBUG(logger, "Invalid std::ofstream constructed with: {}", SETTINGS_FILE_PATH);
		logger->warn("Failed to save settings", path);
		return false;
	}

	file_stream << std::setfill('\t') << std::setw(1) << settings_json << std::endl;
	logger->info("Successfully saved settings");
	return true;
}

data::Settings data::loadSettings(const std::shared_ptr<spdlog::logger>& logger) noexcept
{
	data::Settings settings;
	settings.explorer_folder = DEFAULT_EXPLORER_FOLDER;

	nlohmann::json settings_json;
	std::ifstream file_stream(SETTINGS_FILE_PATH);
	if(!file_stream)
	{
		SPDLOG_DEBUG(logger, "Invalid std::ifstream constructed with: {}", SETTINGS_FILE_PATH);
		logger->info("Failed to load saved settings, default settings will be used");
		return settings;
	}
	file_stream >> settings_json;
	if(settings_json.empty())
	{
		logger->warn("Invalid saved settings, default settings will be used");
		return settings;
	}

	nlohmann::json::iterator it = settings_json.find("explorer_folder");
	if(it == settings_json.end())
	{
		logger->info("Failed to load saved explorer folder path, default path will be used");
	}
	else
	{
		if(!it->is_string())
		{
			logger->warn(
			  "Saved settings contains invalid data for explorer folder path, default path will be used");
		}
		else
		{
			std::string path_str = it->get<std::string>();
			std::filesystem::path path;
			if(!utf8_string_to_path(path_str, path))
			{
				logger->warn(
				  "Explorer folder loaded from saved settings have an invalid utf8 path: {}, default path will be used",
				  path_str);
			}
			else
			{
				SPDLOG_DEBUG(logger, "Loaded explorer folder from saved settings: {}", path);
				settings.explorer_folder = std::move(path);
			}
		}
	}

	it = settings_json.find("music_sources");
	if(it != settings_json.end())
	{
		if(!it->is_array())
		{
			logger->warn("Saved settings contains invalid data for music sources");
		}
		else
		{
			for(const nlohmann::json& source: *it)
			{
				if(!source.is_string())
				{
					logger->warn("Saved settings contains invalid data for a music source");
				}
				else
				{
					std::string path_str = source.get<std::string>();
					std::filesystem::path path;
					if(!utf8_string_to_path(path_str, path))
					{
						logger->warn(
						  "Saved settings contains a music source with an invalid utf8 path: {}, the source will be ignored",
						  path_str);
					}
					else
					{
						SPDLOG_DEBUG(logger, "Loaded music source from saved settings: {}", path);
						settings.music_sources.push_back(std::move(path));
					}
				}
			}
		}
	}

	logger->info("Loaded settings");
	return settings;
}
