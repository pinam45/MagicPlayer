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

#include <SoundFileReaderMp3.hpp>

#include <SFML/Audio/SoundFileFactory.hpp>

#define UNUSED_PARAMETER(x) (void)(x)

namespace {
	std::once_flag SFML_inited;
	void init_SFML(){
		std::call_once(SFML_inited, [](){
			sf::SoundFileFactory::registerReader<SoundFileReaderMp3>();
		});
	}

	void sendMusicOffset(Msg::Com& com, sf::Music& music) {
		com.out.emplace_back(
		  std::in_place_type_t<Msg::Out::MusicOffset>{},
		  music.getPlayingOffset().asSeconds()
		);
	}
}

Logic::Logic() : m_end(false) {
	init_SFML();
}

template<>
void Logic::handleMessage(Msg::In::Close& message) {
	UNUSED_PARAMETER(message);
	m_end = true;
}

template<>
void Logic::handleMessage(Msg::In::Load& message) {
	m_music.stop();
	if(m_music.openFromFile(message.path)){
		m_com.out.push_back(
		  Msg::Out::MusicInfo(
		    true,
		    m_music.getDuration().asSeconds()
		  )
		);
	}
	else{
		m_com.out.push_back(
		  Msg::Out::MusicInfo(
			false,
			0
		  )
		);
	}
}

template<>
void Logic::handleMessage(Msg::In::Control& message) {
	switch(message.action){
		case Msg::In::Control::PLAY:
			if(m_music.getStatus() == sf::Music::Stopped){
				// really stop music if just ended
				m_music.stop();
			}
			m_music.play();
			break;
		case Msg::In::Control::PAUSE:
			m_music.pause();
			break;
		case Msg::In::Control::STOP:
			m_music.stop();
			break;
	}
}

template<>
void Logic::handleMessage(Msg::In::Volume& message) {
	if(message.muted){
		m_music.setVolume(0);
		return;
	}

	if(message.volume >= 0 && message.volume <= 100){
		m_music.setVolume(message.volume);
	}
}

template<>
void Logic::handleMessage(Msg::In::MusicOffset& message) {
	if(message.seconds <= m_music.getDuration().asSeconds()){
		m_music.setPlayingOffset(sf::seconds(message.seconds));
	}
	sendMusicOffset(m_com, m_music);
}

template<>
void Logic::handleMessage(Msg::In::RequestMusicOffset& message) {
	UNUSED_PARAMETER(message);
	sendMusicOffset(m_com, m_music);
}

Msg::Com& Logic::getCom() {
	return m_com;
}

void Logic::run() {
	while(!m_end){
		Msg::Com::InMessage message_ = m_com.in.front();
		std::visit([&](auto&& message) noexcept {
			handleMessage(message);
		}, message_);
		m_com.in.pop_front();
	}
}
