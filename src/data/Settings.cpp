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

data::Settings::Settings() noexcept: explorer_folder(DEFAULT_EXPLORER_FOLDER)
{
}

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

bool data::saveSettings(data::Settings& settings,
                        const std::shared_ptr<spdlog::logger>& logger) noexcept
{
	nlohmann::json settings_json;
	std::error_code error;
	utf8_path path = std::filesystem::canonical(settings.explorer_folder.path(), error);
	if(error)
	{
		SPDLOG_DEBUG(logger, "std::filesystem::canonical failed: {}", error.message());
		logger->warn("Failed to get canonical path of explorer folder {}, path will be saved as is",
		             settings.explorer_folder);
		path = settings.explorer_folder;
	}
	if(!path.valid())
	{
		logger->warn("Explorer folder have an invalid utf8 path and can't be saved: {}", path);
	}
	else
	{
		settings_json["explorer_folder"] = path.str();
		settings.explorer_folder = std::move(path); // apply canonical to settings in memory
	}

	std::vector<std::string> music_sources_str;
	music_sources_str.reserve(settings.music_sources.size());
	for(utf8_path& music_source: settings.music_sources)
	{
		path = std::filesystem::canonical(music_source.path(), error);
		if(error)
		{
			SPDLOG_DEBUG(logger, "std::filesystem::canonical failed: {}", error.message());
			logger->warn(
			  "Failed to get canonical path of music source {}, path will be saved as is",
			  music_source);
			path = music_source;
		}
		if(!path.valid())
		{
			logger->warn("Music source have an invalid utf8 path and can't be saved: {}", path);
		}
		else
		{
			music_sources_str.emplace_back(path.str());
			music_source = std::move(path); // apply canonical to settings in memory
		}
	}
	settings_json["music_sources"] = std::move(music_sources_str);

	std::ofstream file_stream(SETTINGS_FILE_PATH);
	if(!file_stream)
	{
		SPDLOG_DEBUG(logger, "Invalid std::ofstream constructed with: {}", SETTINGS_FILE_PATH);
		logger->warn("Failed to save settings");
		return false;
	}

	file_stream << std::setfill('\t') << std::setw(1) << settings_json << std::endl;
	logger->info("Successfully saved settings");
	return true;
}

data::Settings data::loadSettings(const std::shared_ptr<spdlog::logger>& logger) noexcept
{
	// Load json
	data::Settings settings;
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

	// Load explorer folder
	utf8_path explorer_folder;
	bool loaded_explorer_folder = false;
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
			settings.explorer_folder = it->get<std::string>();
			if(!settings.explorer_folder.valid())
			{
				logger->warn(
				  "Explorer folder loaded from saved settings contains invalid utf8 characters: {}, default path will be used",
				  settings.explorer_folder);
			}
			else
			{
				SPDLOG_DEBUG(logger,
				             "Loaded explorer folder from saved settings: {}",
				             settings.explorer_folder);
				loaded_explorer_folder = true;
			}
		}
	}
	if(!loaded_explorer_folder)
	{
		std::error_code error;
		settings.explorer_folder = std::filesystem::canonical(DEFAULT_EXPLORER_FOLDER, error);
		if(error)
		{
			SPDLOG_DEBUG(logger, "std::filesystem::canonical failed: {}", error.message());
			logger->warn("Failed to get canonical path of default explorer folder {}",
			             DEFAULT_EXPLORER_FOLDER);
			settings.explorer_folder = DEFAULT_EXPLORER_FOLDER;
		}
		if(!settings.explorer_folder.valid())
		{
			logger->warn("Default explorer folder contains invalid utf8 characters: {}",
			             settings.explorer_folder);
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
					utf8_path path = source.get<std::string>();
					if(!path.valid())
					{
						logger->warn(
						  "Saved settings contains a music source with an invalid utf8 path: {}, the source will be ignored",
						  path);
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

bool data::equivalent_sources(const std::vector<utf8_path>& sources_a,
                              const std::vector<utf8_path>& sources_b,
                              const std::shared_ptr<spdlog::logger>& logger) noexcept
{
	std::vector<bool> b_matched;
	b_matched.resize(sources_b.size());

	std::error_code error;
	for(size_t i = 0; i < sources_b.size(); ++i)
	{
		if(!std::filesystem::exists(sources_b[i].path(), error))
		{
			b_matched[i] = true;
		}
	}

	for(const utf8_path& path: sources_a)
	{
		if(!std::filesystem::exists(path.path(), error))
		{
			if(error)
			{
				SPDLOG_DEBUG(logger, "std::filesystem::exists failed: {}", error.message());
				logger->warn("Check if folder exist failed for {}", path);
			}
			else
			{
				logger->warn("Music source folder doesn't exist: {}", path);
			}
			continue;
		}

		bool matched_b = false;
		for(size_t i = 0; i < sources_b.size(); ++i)
		{
			if(std::filesystem::equivalent(path.path(), sources_b[i].path(), error))
			{
				b_matched[i] = true;
				matched_b = true;
			}
			if(error)
			{
				SPDLOG_DEBUG(logger, "std::filesystem::equivalent failed: {}", error.message());
				logger->warn("Path comparison failed for {} and {}", path, sources_b[i]);
			}
		}
		if(!matched_b)
		{
			return false;
		}
	}

	// std::identity is C++20...
	return std::all_of(
	  std::cbegin(b_matched), std::cend(b_matched), [](bool matched) { return matched; });
}
