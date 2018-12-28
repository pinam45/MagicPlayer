//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_DATAMANAGER_HPP
#define MAGICPLAYER_DATAMANAGER_HPP

#include "data/Database.hpp"
#include "utils/IdGenerator.hpp"

#include <spdlog/logger.h>

#include <memory>

namespace data
{
	class DataManager
	{
	public:
		explicit DataManager(std::shared_ptr<spdlog::logger> logger) noexcept;

		DataManager(const DataManager&) = delete;
		DataManager& operator=(const DataManager&) = delete;

		DataManager(DataManager&&) = delete;
		DataManager& operator=(DataManager&&) = delete;

		~DataManager() noexcept = default;

		[[nodiscard]] std::shared_ptr<const Database> generateDatabase(
		  const std::vector<std::filesystem::path>& sources);
		[[nodiscard]] std::shared_ptr<const Database> loadDatabase();
		[[nodiscard]] bool saveDatabase(std::shared_ptr<const Database>& database);

	private:
		struct Cache
		{
			Artist* artist = nullptr;
			Album* album = nullptr;
		};

		bool loadMusicFromFolder(std::filesystem::path folder_path,
		                         std::shared_ptr<Database>& database);

		bool loadMusicFromFile(std::filesystem::path file_path,
		                       std::shared_ptr<Database>& database,
		                       Cache& cache);

		void attributeIds(std::shared_ptr<Database>& database);

		[[nodiscard]] Artist* getArtist(std::string_view name,
		                                std::shared_ptr<data::Database>& database,
		                                Cache& cache);

		[[nodiscard]] Album* getAlbum(std::string_view name, Artist& artist, Cache& cache);

		Artist* addArtist(Artist&& artist, std::shared_ptr<data::Database>& database, Cache& cache);

		Album* addAlbum(Album&& album, Artist& artist, Cache& cache);

		Music* addMusic(Music&& music, Album& album);

		IdGenerator m_idGenerator;
		std::shared_ptr<spdlog::logger> m_logger;
	};
} // namespace data

#endif //MAGICPLAYER_DATAMANAGER_HPP
