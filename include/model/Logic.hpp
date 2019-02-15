//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_LOGIC_HPP
#define MAGICPLAYER_LOGIC_HPP

#include "model/Messages.hpp"
#include "data/Database.hpp"
#include "data/DataManager.hpp"
#include "utils/path_utils.hpp"

#include <SFML/Audio/Music.hpp>
#include <spdlog/logger.h>

#include <future>

class Logic final
{

public:
	Logic();

	~Logic();

	Msg::Com& getCom();

	void run();

private:
	// input checked
	template<typename Message>
	void handleMessage(Message& message) = delete;

	// input not checked
	void loadFile(const utf8_path& path);

	void sendFolderContent(const std::filesystem::path& path);

	void async_sendFolderContent(const std::filesystem::path& path);

	void async_loadSettings();

	void async_loadDatabase();

	void async_generateDatabase(std::vector<utf8_path> music_sources);

	template<typename Lambda, typename... Parameters>
	void async_task(Lambda lambda, Parameters... parameters);

	// variables
	std::shared_ptr<spdlog::logger> m_logger;

	Msg::Com m_com;
	bool m_end;
	sf::Music m_music;
	std::vector<std::future<std::packaged_task<void()>>> m_pending_futures;
	data::Settings m_settings;
	data::DataManager m_data_manager;
	std::shared_ptr<const data::Database> m_database;
};

#endif //MAGICPLAYER_LOGIC_HPP
