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
	template<typename Message>
	void handleMessage(Message& message) = delete;

	void loadFile(std::filesystem::path path);

	void sendFolderContent(std::filesystem::path path);

	void async_loadSettings();

	void async_generateDatabase(std::vector<std::filesystem::path> music_sources);

	Msg::Com m_com;
	bool m_end;
	sf::Music m_music;
	std::vector<std::future<void>> m_pending_futures;
	data::Settings m_settings;
	std::shared_ptr<const data::Database> m_database;

	std::shared_ptr<spdlog::logger> m_logger;
};

#endif //MAGICPLAYER_LOGIC_HPP
