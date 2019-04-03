//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "data/DataManager.hpp"
#include "utils/log.hpp"
#include "utils/audio_extensions.hpp"

#include <spdlog/spdlog.h>
#include <taglib/tag.h>
#include <taglib/fileref.h>

#include <queue>
#include <future>

namespace
{
	bool compareArtists(const data::Artist& left, const data::Artist& right)
	{
		return left.name < right.name;
	}

	bool compareAlbums(const data::Album& left, const data::Album& right)
	{
		// reverse year order
		return std::make_tuple(-left.year, left.name) < std::make_tuple(-right.year, right.name);
	}

	bool compareMusics(const data::Music& left, const data::Music& right)
	{
		return std::make_tuple(left.track, left.title) < std::make_tuple(right.track, right.title);
	}

	void sortAlbum(data::Album& album)
	{
		std::sort(album.musics.begin(), album.musics.end(), &compareMusics);
	}

	void sortArtist(data::Artist& artist)
	{
		std::vector<data::Album>& albums = artist.albums;
		std::sort(albums.begin(), albums.end(), &compareAlbums);

		std::vector<std::future<void>> futures;
		futures.reserve(albums.size());
		for(data::Album& album: albums)
		{
			futures.push_back(std::async(&sortAlbum, std::ref(album)));
		}
		for(std::future<void>& future: futures)
		{
			future.wait();
		}
	}

	void sortDatabase(std::shared_ptr<data::Database>& database)
	{
		std::vector<data::Artist>& artists = database->artists;
		std::sort(artists.begin(), artists.end(), &compareArtists);

		std::vector<std::future<void>> futures;
		futures.reserve(artists.size());
		for(data::Artist& artist: artists)
		{
			futures.push_back(std::async(&sortArtist, std::ref(artist)));
		}
		for(std::future<void>& future: futures)
		{
			future.wait();
		}
	}
} // namespace

data::DataManager::DataManager(std::shared_ptr<spdlog::logger> logger) noexcept
  : m_logger(std::move(logger))
{
}

std::shared_ptr<const data::Database> data::DataManager::generateDatabase(
  const std::vector<utf8_path>& sources)
{
	// no std::make_shared: friend class with private constructor
	std::shared_ptr<Database> database(new Database());
	m_logger->info("Started database generation");

	for(const utf8_path& path: sources)
	{
		if(!path.valid_encoding())
		{
			m_logger->warn("Music source folder contains invalid utf8 characters: {}", path);
			continue;
		}
		std::error_code error;
		if(!std::filesystem::exists(path.path(), error))
		{
			if(error)
			{
				SPDLOG_DEBUG(m_logger, "std::filesystem::exists failed: {}", error.message());
				m_logger->warn("Check if folder exist failed for {}", path);
			}
			else
			{
				m_logger->warn("Music source folder doesn't exist: {}", path);
			}
			continue;
		}
		if(!std::filesystem::is_directory(path.path(), error))
		{
			if(error)
			{
				SPDLOG_DEBUG(m_logger, "std::filesystem::is_directory failed: {}", error.message());
				m_logger->warn("Check if path is a folder failed for {}", path);
			}
			else
			{
				m_logger->warn("Music source is not a folder: {}", path);
			}
			continue;
		}
		if(!loadMusicFromFolder(path.path(), database))
		{
			m_logger->warn("Incomplete music loading from: {}", path);
		}
		database->sources.push_back(path);
	}

	sortDatabase(database);
	attributeIds(database);

	database->generation_date = std::chrono::system_clock::now();
	m_logger->info("Finished database generation");
	return database;
}

std::shared_ptr<const data::Database> data::DataManager::loadDatabase()
{
	//TODO
	m_logger->error("Not implemented: data::DataManager::loadDatabase()");

	// no std::make_shared: friend class with private constructor
	std::shared_ptr<Database> database(new Database());
	database->id = 0;
	database->generation_date = std::chrono::system_clock::now();
	return database;
}

bool data::DataManager::saveDatabase([
  [maybe_unused]] std::shared_ptr<const data::Database>& database)
{
	//TODO
	m_logger->error("Not implemented: data::DataManager::saveDatabase()");
	return false;
}

bool data::DataManager::loadMusicFromFolder(const std::filesystem::path& folder_path,
                                            std::shared_ptr<data::Database>& database)
{
	std::queue<std::filesystem::path> folders;
	folders.push(folder_path);

	Cache cache;
	bool load_success = true;
	while(!folders.empty())
	{
		std::filesystem::path folder = folders.front();
		folders.pop();
		SPDLOG_TRACE(m_logger, "Database generation: processing folder {}", folder);

		std::error_code error;
		std::filesystem::directory_iterator directory_iterator(folder, error);
		if(error)
		{
			SPDLOG_DEBUG(m_logger, "Directory iterator creation failed: {}", error.message());
			m_logger->warn("Failed to get content of folder {}", folder);
			load_success = false;
			continue;
		}
		for(const std::filesystem::directory_entry& entry: directory_iterator)
		{
			std::error_code directory_error;
			std::error_code file_error;
			if(entry.is_directory(directory_error))
			{
				folders.push(entry.path());
			}
			else if(entry.is_regular_file(file_error))
			{
				load_success |= loadMusicFromFile(entry.path(), database, cache);
			}
			else
			{
				if(directory_error)
				{
					SPDLOG_DEBUG(m_logger,
					             "is_directory failed on entry {}: {}",
					             entry.path(),
					             directory_error.message());
				}
				if(file_error)
				{
					SPDLOG_DEBUG(m_logger,
					             "is_regular_file failed on entry {}: {}",
					             entry.path(),
					             file_error.message());
				}
				if(directory_error || file_error)
				{
					load_success = false;
					m_logger->warn("File/folder type determination failed for {}", entry.path());
				}
			}
		}
	}
	return load_success;
}

bool data::DataManager::loadMusicFromFile(const std::filesystem::path& file_path,
                                          std::shared_ptr<data::Database>& database,
                                          Cache& cache)
{
	if(!hasSupportedAudioExtension(file_path))
	{
		return true;
	}
	SPDLOG_TRACE(m_logger, "Database generation: processing file {}", file_path);

	TagLib::FileRef fileref(file_path.native().c_str());
	if(fileref.isNull())
	{
		m_logger->warn("Failed to load file {}", file_path);
		return false;
	}
	TagLib::Tag* tags = fileref.tag();
	if(tags == nullptr)
	{
		m_logger->warn("Failed to load file tags {}", file_path);
		return false;
	}

	std::string artist_name = tags->artist().to8Bit(true);
	Artist* artist = getArtist(artist_name, database, cache);
	if(artist == nullptr)
	{
		artist = addArtist(Artist{artist_name}, database, cache);
	}

	std::string album_name = tags->album().to8Bit(true);
	Album* album = getAlbum(album_name, *artist, cache);
	if(album == nullptr)
	{
		album =
		  addAlbum(Album{album_name, tags->genre().to8Bit(true), static_cast<int>(tags->year())},
		           *artist,
		           cache);
		//TODO: image
	}

	std::chrono::duration<int> length = std::chrono::seconds(0);
	TagLib::AudioProperties* audioProperties = fileref.audioProperties();
	if(audioProperties == nullptr)
	{
		m_logger->warn("Failed to read audio properties from {}", file_path);
	}
	else
	{
		length = std::chrono::seconds(audioProperties->lengthInSeconds());
	}
	addMusic(Music{static_cast<int>(tags->track()), tags->title().to8Bit(true), length, file_path},
	         *album);

	return true;
}

void data::DataManager::attributeIds(std::shared_ptr<data::Database>& database)
{
	std::lock_guard<IdGenerator> guard(m_idGenerator);

	assert(database->id == IdGenerator::INVALID_ID);
	database->id = m_idGenerator.next_id();
	std::vector<Artist>& artists = database->artists;
	for(Artist& artist: artists)
	{
		artist.id = m_idGenerator.next_id();
		std::vector<Album>& albums = artist.albums;
		for(Album& album: albums)
		{
			album.id = m_idGenerator.next_id();
			std::vector<Music>& musics = album.musics;
			for(Music& music: musics)
			{
				assert(music.id == IdGenerator::INVALID_ID);
				music.id = m_idGenerator.next_id();
			}
		}
	}
}

data::Artist* data::DataManager::getArtist(std::string_view name,
                                           std::shared_ptr<data::Database>& database,
                                           data::DataManager::Cache& cache)
{
	if(cache.artist != nullptr)
	{
		if(name == cache.artist->name)
		{
			return cache.artist;
		}
	}

	auto it = std::find_if(database->artists.begin(),
	                       database->artists.end(),
	                       [&name](const Artist& artist) { return artist.name == name; });
	if(it != database->artists.end())
	{
		cache.artist = &*it;
		return cache.artist;
	}

	return nullptr;
}

data::Album* data::DataManager::getAlbum(std::string_view name,
                                         data::Artist& artist,
                                         data::DataManager::Cache& cache)
{
	if(cache.album != nullptr)
	{
		if(name == cache.album->name)
		{
			return cache.album;
		}
	}

	auto it = std::find_if(artist.albums.begin(), artist.albums.end(), [&name](const Album& album) {
		return album.name == name;
	});
	if(it != artist.albums.end())
	{
		cache.album = &*it;
		return cache.album;
	}

	return nullptr;
}

data::Artist* data::DataManager::addArtist(data::Artist&& artist,
                                           std::shared_ptr<data::Database>& database,
                                           data::DataManager::Cache& cache)
{
	database->artists.push_back(std::move(artist));
	cache.artist = &database->artists.back();
	return cache.artist;
}

data::Album* data::DataManager::addAlbum(data::Album&& album,
                                         data::Artist& artist,
                                         data::DataManager::Cache& cache)
{
	artist.addAlbum(std::move(album));
	cache.album = &artist.albums.back();
	return cache.album;
}

data::Music* data::DataManager::addMusic(data::Music&& music, data::Album& album)
{
	album.addMusic(std::move(music));
	return &album.musics.back();
}
