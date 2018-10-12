/*****************************************************************************************
 *                                                                                       *
 * MIT License                                                                           *
 *                                                                                       *
 * Copyright (c) 2018 Maxime Pinard                                                      *
 *                                                                                       *
 * Permission is hereby granted, free of charge, to any person obtaining a copy          *
 * of this software and associated documentation files (the "Software"), to deal         *
 * in the Software without restriction, including without limitation the rights          *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell             *
 * copies of the Software, and to permit persons to whom the Software is                 *
 * furnished to do so, subject to the following conditions:                              *
 *                                                                                       *
 * The above copyright notice and this permission notice shall be included in all        *
 * copies or substantial portions of the Software.                                       *
 *                                                                                       *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR            *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,              *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE           *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER                *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,         *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE         *
 * SOFTWARE.                                                                             *
 *                                                                                       *
 *****************************************************************************************/

#include "model/Logic.hpp"
#include "utils/log.hpp"

#include "SoundFileReaderMp3.hpp"

#include <SFML/Audio/SoundFileFactory.hpp>
#include <spdlog/spdlog.h>

namespace {
	std::once_flag SFML_inited;
	void init_SFML(){
		std::call_once(SFML_inited, [](){
			sf::SoundFileFactory::registerReader<SoundFileReaderMp3>();
			SPDLOG_DEBUG(spdlog::get(GENERAL_LOGGER_NAME), "Custom MP3 file reader registered to SFML SoundFileFactory");
		});
	}
}

template<typename Message, typename... Args>
void Logic::sendMessage(Args&&... args) {
	m_com.out.emplace_back(std::in_place_type_t<Message>{}, std::forward<Args>(args)...);
}


template<>
void Logic::handleMessage([[maybe_unused]] Msg::In::Close& message) {
	SPDLOG_DEBUG(m_logger, "Received close request");
	m_end = true;
}

template<>
void Logic::handleMessage(Msg::In::Load& message) {
	SPDLOG_DEBUG(m_logger, "Received load request: {}", message.path);
	m_music.stop();
	if(m_music.openFromFile(message.path.generic_string())){
		m_logger->info("Loaded {}", message.path);
		sendMessage<Msg::Out::MusicInfo>(true, m_music.getDuration().asSeconds());
	}
	else{
		m_logger->warn("Failed to load {}", message.path);
		sendMessage<Msg::Out::MusicInfo>(false, 0);
	}
}

template<>
void Logic::handleMessage(Msg::In::Control& message) {
	SPDLOG_DEBUG(m_logger, "Received control request: {}", message.action);
	switch(message.action){
		case Msg::In::Control::PLAY:
			if(m_music.getStatus() == sf::Music::Stopped){
				// really stop music if just ended
				m_music.stop();
			}
			m_music.play();
			m_logger->info("Music played/resumed");
			break;
		case Msg::In::Control::PAUSE:
			m_music.pause();
			m_logger->info("Music paused");
			break;
		case Msg::In::Control::STOP:
			m_music.stop();
			m_logger->info("Music stopped");
			break;
	}
}

template<>
void Logic::handleMessage(Msg::In::Volume& message) {
	SPDLOG_DEBUG(m_logger, "Received volume request: {:.2f}% {}muted", message.volume, message.muted ? "" : "not ");
	if(message.muted){
		m_music.setVolume(0);
		m_logger->info("Volume muted");
		return;
	}

	if(message.volume >= 0 && message.volume <= 100){
		m_music.setVolume(message.volume);
		m_logger->info("Volume set to {:.2f}%", message.volume);
	}
	else{
		m_logger->warn("Invalid volume value requested: {:.2f}%", message.volume);
	}
}

template<>
void Logic::handleMessage(Msg::In::MusicOffset& message) {
	SPDLOG_DEBUG(m_logger, "Received music offset request: {:.2f} seconds", message.seconds);
	if(message.seconds <= m_music.getDuration().asSeconds()){
		m_music.setPlayingOffset(sf::seconds(message.seconds));
		m_logger->info("Set music offset to {:.2f} seconds", message.seconds);
	}
	else{
		m_logger->warn("Invalid music offset requested: {:.2f} seconds", message.seconds);
	}
	sendMessage<Msg::Out::MusicOffset>(m_music.getPlayingOffset().asSeconds());
}

template<>
void Logic::handleMessage([[maybe_unused]] Msg::In::RequestMusicOffset& message) {
	sendMessage<Msg::Out::MusicOffset>(m_music.getPlayingOffset().asSeconds());
}

Logic::Logic() : m_end(false), m_logger(spdlog::get(LOGIC_LOGGER_NAME)) {
	init_SFML();
}

Msg::Com& Logic::getCom() {
	return m_com;
}

void Logic::run() {
	while(!m_end){
		Msg::Com::InMessage message_ = m_com.in.front();
		std::visit([&](auto&& message) noexcept {
			SPDLOG_TRACE(m_logger, "Received message {}", message);
			handleMessage(message);
		}, message_);
		m_com.in.pop_front();
	}
	SPDLOG_DEBUG(m_logger, "Main loop ended");
}
