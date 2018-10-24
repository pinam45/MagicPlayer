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

class Logic {

public:

	Logic();

	Msg::Com& getCom();

	void run();

private:

	template<typename Message, typename... Args>
	void sendMessage(Args&&... args);

	template<typename Message>
	void handleMessage(Message& message) = delete;

	Msg::Com m_com;
	bool m_end;
	sf::Music m_music;

	std::shared_ptr<spdlog::logger> m_logger;
};


#endif //MAGICPLAYER_LOGIC_HPP
