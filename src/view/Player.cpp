//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "view/Player.hpp"
#include "view/imgui_custom_widgets.hpp"
#include "view/FontManager.hpp"
#include "utils/log.hpp"

#include <IconsFontAwesome5.h>
#include <imgui.h>

namespace
{
	constexpr float MUSIC_OFFSET_REFRESH_SECONDS = 0.05f;
	constexpr float MUSIC_INITIAL_VOLUME = 20.0f;
} // namespace

Player::Player(std::string name, Msg::Sender sender)
  : m_sender(sender)
  , m_name(std::move(name))
  , m_musicInfos()
  , m_volume(MUSIC_INITIAL_VOLUME)
  , m_musicOffsetClock()
  , m_logger(spdlog::get(VIEW_LOGGER_NAME))
{
}

void Player::init()
{
	m_sender.sendInMessage<Msg::In::Volume>(false, m_volume);
}

void Player::show()
{
	// Request music offset
	if(m_musicInfos.valid
	   && m_musicOffsetClock.getElapsedTime() > sf::seconds(MUSIC_OFFSET_REFRESH_SECONDS))
	{
		m_sender.sendInMessage<Msg::In::RequestMusicOffset>();
		m_musicOffsetClock.restart();
	}

	ImGui::Begin(m_name.c_str());
	FontManager::pushFontSize(FontManager::LARGE_FONT_SIZE);

	if(m_musicInfos.valid)
	{
		// Control buttons
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4());
		if(ImGui::Button(ICON_FA_PLAY))
		{
			m_logger->info("Request to play/resume music");
			m_sender.sendInMessage<Msg::In::Control>(Msg::In::Control::Action::PLAY);
		}
		ImGui::SameLine();
		if(ImGui::Button(ICON_FA_PAUSE))
		{
			m_logger->info("Request to pause music");
			m_sender.sendInMessage<Msg::In::Control>(Msg::In::Control::Action::PAUSE);
		}
		ImGui::SameLine();
		if(ImGui::Button(ICON_FA_STOP))
		{
			m_logger->info("Request to stop music");
			m_sender.sendInMessage<Msg::In::Control>(Msg::In::Control::Action::STOP);
		}
		ImGui::PopStyleColor();

		// Song playing offset
		const int playingOffset_i = static_cast<int>(m_musicInfos.offset);
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetFontSize() * 2.7f);
		ImGui::LabelText("##time_pre", "%02d:%02d", playingOffset_i / 60, playingOffset_i % 60);
		ImGui::PopItemWidth();

		// Song playing bar
		float trac_pos = m_musicInfos.offset;
		ImVec2 player_bar_size(ImGui::GetWindowContentRegionWidth() - ImGui::GetFontSize() * 20.0f,
		                       0.0f);
		ImGui::SameLine();
		if(ImGuiCW::PlayerBar(
		     "player_bar", &trac_pos, 0.0f, m_musicInfos.duration, player_bar_size))
		{
			m_logger->info("Request to set music offset to {:.2f} seconds", trac_pos);
			m_sender.sendInMessage<Msg::In::MusicOffset>(trac_pos);
		}

		// Song duration
		const int duration_i = static_cast<int>(m_musicInfos.duration);
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetFontSize() * 2.7f);
		ImGui::LabelText("##time_post", "%02d:%02d", duration_i / 60, duration_i % 60);
		ImGui::PopItemWidth();

		ImGui::SameLine();
	}

	// Volume
	ImGui::PushItemWidth(ImGui::GetFontSize() * 1.3f);
	ImGui::LabelText("##volume_icon", ICON_FA_VOLUME_UP);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::PushItemWidth(-1);
	if(ImGui::SliderFloat("##volume", &m_volume, 0.f, 100.f, "%.0f%%"))
	{
		m_logger->info("Request to change volume to {:.2f}%", m_volume);
		m_sender.sendInMessage<Msg::In::Volume>(false, m_volume);
	}
	ImGui::PopItemWidth();

	FontManager::popFontSize();
	ImGui::End();
}

void Player::processMessage(Msg::Out::MusicOffset& message)
{
	m_musicInfos.offset = message.seconds;
}

void Player::processMessage(Msg::Out::MusicInfo& message)
{
	m_musicInfos.valid = message.valid;
	m_musicInfos.offset = 0;
	m_musicInfos.duration = message.durationSeconds;
	m_logger->info("Received music information: valid = {}, duration = {:.2f} seconds",
	               m_musicInfos.valid,
	               m_musicInfos.duration);
	m_sender.sendInMessage<Msg::In::RequestMusicOffset>();
}
