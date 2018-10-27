//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_LOGIC_HPP
#define MAGICPLAYER_LOGIC_HPP

#include "Messages.hpp"

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
	template<typename Message, typename... Args>
	void sendMessage(Args&&... args);

	template<typename Message>
	void handleMessage(Message& message) = delete;

	void loadFile(std::filesystem::path path);

	void sendFolderContent(std::filesystem::path path);

	Msg::Com m_com;
	bool m_end;
	sf::Music m_music;
	std::vector<std::future<void>> m_pending_futures; //TODO: change collection?

	std::shared_ptr<spdlog::logger> m_logger;
};

#endif //MAGICPLAYER_LOGIC_HPP
