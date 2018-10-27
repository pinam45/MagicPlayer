//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_GUI_HPP
#define MAGICPLAYER_GUI_HPP

#include "model/Messages.hpp"
#include "imgui_easy_theming.hpp"

#include <imgui.h>
#include <spdlog/logger.h>

#include <array>

class GUI final
{

public:
	explicit GUI(Msg::Com& com);

	int run();

private:
	template<typename Message, typename... Args>
	void sendMessage(Args&&... args);

	template<typename Message>
	void handleMessage(Message& message) = delete;

	void show();
	void showMainDockspace();
	void showPlayer();
	void showExplorer();

	ImFont* loadFonts(float pixel_size);

	void loadInitialConfig();

	void setupImGui();
	void setupFonts();
	void setupStyle();

	void processMessages();

	struct MusicInfos
	{
		bool valid;
		float offset;
		float duration;

		MusicInfos() noexcept = default;
	};
	MusicInfos m_musicInfos;

	Msg::Com& m_com;
	bool m_showThemeConfigWindow;
	std::array<char, 2018> m_music_file_path;
	float m_volume;
	ImGui::ETheming::ColorTheme m_style;
	ImFont* m_normal_font;
	ImFont* m_large_font;

	std::filesystem::path m_explorer_path;
	std::vector<PathInfo> m_explorer_content;

	std::shared_ptr<spdlog::logger> m_logger;
};

#endif //MAGICPLAYER_GUI_HPP
