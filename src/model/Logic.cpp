//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "model/Logic.hpp"
#include "utils/log.hpp"

#include "SoundFileReaderMp3.hpp"

#include <SFML/Audio/SoundFileFactory.hpp>
#include <spdlog/spdlog.h>

#include <thread>
#include <future>

namespace
{
	constexpr std::array<std::string_view, 4> SUPPORTED_AUDIO_EXTENSIONS = {".vaw",
	                                                                        ".ogg",
	                                                                        ".flac",
	                                                                        ".mp3"};

	std::once_flag SFML_inited;
	void init_SFML()
	{
		std::call_once(SFML_inited, []() {
			sf::SoundFileFactory::registerReader<SoundFileReaderMp3>();
			SPDLOG_DEBUG(spdlog::get(GENERAL_LOGGER_NAME),
			             "Custom MP3 file reader registered to SFML SoundFileFactory");
		});
	}
} // namespace

template<typename Message, typename... Args>
void Logic::sendMessage(Args&&... args)
{
	m_com.out.emplace_back(std::in_place_type_t<Message>{}, std::forward<Args>(args)...);
}

template<>
void Logic::handleMessage([[maybe_unused]] Msg::In::Close& message)
{
	SPDLOG_DEBUG(m_logger, "Received close request");
	m_end = true;
}

template<>
void Logic::handleMessage(Msg::In::Open& message)
{
	SPDLOG_DEBUG(m_logger, "Received open request: {}", message.path);

	std::error_code error;
	if(!std::filesystem::exists(message.path, error))
	{
		if(error)
		{
			SPDLOG_DEBUG(m_logger, "std::filesystem::exists failed: {}", error.message());
			m_logger->warn("Check if file/folder exist failed for {}", message.path);
		}
		else
		{
			m_logger->warn("Open request non-existing file/folder {}", message.path);
		}
		return;
	}

	if(std::filesystem::is_regular_file(message.path, error))
	{
		loadFile(message.path);
		return;
	}
	if(error)
	{
		SPDLOG_DEBUG(m_logger, "std::filesystem::is_regular_file failed: {}", error.message());
		m_logger->warn("Check if path is a regular file failed for: {}", message.path);
	}

	if(std::filesystem::is_directory(message.path, error))
	{
		auto process = [this](const std::filesystem::path path) noexcept
		{
			sendFolderContent(path);
			m_com.in.emplace_back(std::in_place_type_t<Msg::In::InnerTaskEnded>{});
		};
		m_pending_futures.push_back(std::async(std::launch::async, process, message.path));
		return;
	}
	if(error)
	{
		SPDLOG_DEBUG(m_logger, "std::filesystem::is_directory failed: {}", error.message());
		m_logger->warn("Check if path is a directory failed for: {}", message.path);
	}

	m_logger->warn("Open request on invalid file/folder {}", message.path);
}

template<>
void Logic::handleMessage(Msg::In::Control& message)
{
	SPDLOG_DEBUG(m_logger, "Received control request: {}", message.action);
	switch(message.action)
	{
		case Msg::In::Control::Action::PLAY:
			if(m_music.getStatus() == sf::Music::Stopped)
			{
				// really stop music if just ended
				m_music.stop();
			}
			m_music.play();
			m_logger->info("Music played/resumed");
			break;
		case Msg::In::Control::Action::PAUSE:
			m_music.pause();
			m_logger->info("Music paused");
			break;
		case Msg::In::Control::Action::STOP:
			m_music.stop();
			m_logger->info("Music stopped");
			break;
	}
}

template<>
void Logic::handleMessage(Msg::In::Volume& message)
{
	SPDLOG_DEBUG(m_logger,
	             "Received volume request: {:.2f}% {}muted",
	             message.volume,
	             message.muted ? "" : "not ");
	if(message.muted)
	{
		m_music.setVolume(0);
		m_logger->info("Volume muted");
		return;
	}

	if(message.volume >= 0 && message.volume <= 100)
	{
		m_music.setVolume(message.volume);
		m_logger->info("Volume set to {:.2f}%", message.volume);
	}
	else
	{
		m_logger->warn("Invalid volume value requested: {:.2f}%", message.volume);
	}
}

template<>
void Logic::handleMessage(Msg::In::MusicOffset& message)
{
	SPDLOG_DEBUG(m_logger, "Received music offset request: {:.2f} seconds", message.seconds);
	if(message.seconds <= m_music.getDuration().asSeconds())
	{
		m_music.setPlayingOffset(sf::seconds(message.seconds));
		m_logger->info("Set music offset to {:.2f} seconds", message.seconds);
	}
	else
	{
		m_logger->warn("Invalid music offset requested: {:.2f} seconds", message.seconds);
	}
	sendMessage<Msg::Out::MusicOffset>(m_music.getPlayingOffset().asSeconds());
}

template<>
void Logic::handleMessage([[maybe_unused]] Msg::In::RequestMusicOffset& message)
{
	sendMessage<Msg::Out::MusicOffset>(m_music.getPlayingOffset().asSeconds());
}

template<>
void Logic::handleMessage([[maybe_unused]] Msg::In::InnerTaskEnded& message)
{
	auto it = std::find_if(m_pending_futures.begin(),
	                       m_pending_futures.end(),
	                       [](const std::future<void>& future) { return future.valid(); });
	if(it == std::end(m_pending_futures))
	{
		m_logger->warn("Task ended message received but no valid future found");
		return;
	}
	m_pending_futures.erase(it);
	SPDLOG_TRACE(m_logger, "Inner task ended: erased future");
}

Logic::Logic()
  : m_com(), m_end(false), m_music(), m_pending_futures(), m_logger(spdlog::get(LOGIC_LOGGER_NAME))
{
	init_SFML();
}

Logic::~Logic()
{
	SPDLOG_DEBUG(m_logger, "Start ending all background tasks");
	for(const std::future<void>& future: m_pending_futures)
	{
		SPDLOG_TRACE(m_logger, "Waiting on pending futures");
		future.wait();
	}
	SPDLOG_DEBUG(m_logger, "All background tasks ended");
}

Msg::Com& Logic::getCom()
{
	return m_com;
}

void Logic::run()
{
	while(!m_end)
	{
		Msg::Com::InMessage message_ = m_com.in.front();
		std::visit(
		  [&](auto&& message) noexcept {
			  SPDLOG_TRACE(m_logger, "Received message {}", message);
			  handleMessage(message);
		  },
		  message_);
		m_com.in.pop_front();
	}
	SPDLOG_DEBUG(m_logger, "Main loop ended");
}

void Logic::loadFile(std::filesystem::path path)
{
	std::string path_str;
	if(!path_to_generic_utf8_string(path, path_str))
	{
		m_logger->warn("Tried to load file with invalid utf8 path: {}",
		               invalid_utf8_path_representation(path));
		sendMessage<Msg::Out::MusicInfo>(false, 0);
		return;
	}

	if(m_music.openFromFile(path_str))
	{
		m_music.play();
		m_logger->info("Loaded {}", path);
		m_logger->info("Music played");
		sendMessage<Msg::Out::MusicInfo>(true, m_music.getDuration().asSeconds());
	}
	else
	{
		m_logger->warn("Failed to load {}", path);
		sendMessage<Msg::Out::MusicInfo>(false, 0);
	}
}

void Logic::sendFolderContent(std::filesystem::path path)
{
	std::error_code error;
	path = std::filesystem::canonical(path, error);
	if(error)
	{
		SPDLOG_DEBUG(m_logger, "std::filesystem::canonical failed: {}", error.message());
		m_logger->warn("Failed to get canonical path of folder {}", path);
		return;
	}

	std::filesystem::directory_iterator directory_iterator(path, error);
	if(error)
	{
		SPDLOG_DEBUG(m_logger, "Directory iterator creation failed: {}", error.message());
		m_logger->warn("Failed to get content of folder {}", path);
		return;
	}

	// Generate infos
	std::vector<PathInfo> path_infos;
	for(const std::filesystem::directory_entry& entry: directory_iterator)
	{
		PathInfo infos;
		infos.path = entry.path();

		std::error_code directory_error;
		std::error_code file_error;
		if(entry.is_directory(directory_error))
		{
			infos.is_folder = true;
			std::filesystem::path folder_name;
			if(infos.path.has_filename())
			{
				// path doesn't end by '/', filename() return the folder name
				folder_name = infos.path.filename();
			}
			else
			{
				// path end by '/', use parent_path to get folder name
				folder_name = infos.path.parent_path().filename();
			}
			if(!path_to_generic_utf8_string(folder_name, infos.file_name))
			{
				m_logger->warn("Folder with invalid utf8 name ignored: {}",
				               invalid_utf8_path_representation(folder_name));
				continue;
			}
		}
		else if(entry.is_regular_file(file_error))
		{
			std::filesystem::path file_name = infos.path.filename();
			if(!path_to_generic_utf8_string(file_name, infos.file_name))
			{
				m_logger->warn("File with invalid utf8 name ignored: {}",
				               invalid_utf8_path_representation(file_name));
				continue;
			}
			infos.file_size = entry.file_size(error);
			infos.has_supported_audio_extension = std::find(std::cbegin(SUPPORTED_AUDIO_EXTENSIONS),
			                                                std::cend(SUPPORTED_AUDIO_EXTENSIONS),
			                                                entry.path().extension().c_str())
			                                      != std::cend(SUPPORTED_AUDIO_EXTENSIONS);
		}
		else
		{
			if(directory_error)
			{
				SPDLOG_DEBUG(
				  m_logger, "is_directory failed on entry {}: {}", entry.path(), error.message());
			}
			if(file_error)
			{
				SPDLOG_DEBUG(m_logger,
				             "is_regular_file failed on entry {}: {}",
				             entry.path(),
				             error.message());
			}
			continue;
		}
		path_infos.push_back(std::move(infos));
	}

	// Sort infos
	std::sort(
	  path_infos.begin(), path_infos.end(), [](const PathInfo& lhs, const PathInfo& rhs) noexcept {
		  return std::make_tuple(!lhs.is_folder, lhs.file_name)
		         < std::make_tuple(!rhs.is_folder, rhs.file_name);
	  });
	sendMessage<Msg::Out::FolderContent>(path, std::move(path_infos));
}
