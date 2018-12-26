//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_PLAYER_HPP
#define MAGICPLAYER_PLAYER_HPP

#include "model/Messages.hpp"

#include <SFML/System/Clock.hpp>
#include <spdlog/logger.h>

#include <memory>
#include <string>

class Player
{
public:
	Player(std::string name, Msg::Sender sender);

	void init();

	void show();

	void processMessage(Msg::Out::MusicOffset& message);
	void processMessage(Msg::Out::MusicInfo& message);

private:
	struct MusicInfos
	{
		bool valid = false;
		float offset = 0.f;
		float duration = 0.f;

		MusicInfos() noexcept = default;
	};

	Msg::Sender m_sender;

	std::string m_name;
	MusicInfos m_musicInfos;
	float m_volume;
	sf::Clock m_musicOffsetClock;

	std::shared_ptr<spdlog::logger> m_logger;
};

#endif //MAGICPLAYER_PLAYER_HPP
