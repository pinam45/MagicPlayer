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
#include "view/imgui_easy_theming.hpp"
#include "view/FileExplorer.hpp"
#include "view/LogViewer.hpp"

#include <imgui.h>
#include <spdlog/logger.h>

#include <array>

class GUI final
{

public:
	explicit GUI(Msg::Com& com);

	int run();

private:
	template<typename Message>
	void handleMessage(Message& message) = delete;

	void show();
	void showMainDockspace();
	void showPlayer();

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
	bool m_showLogViewerWindow;
	float m_volume;
	ImGui::ETheming::ColorTheme m_style;

	FileExplorer m_file_explorer;
	LogViewer m_log_viewer;

	std::shared_ptr<spdlog::logger> m_logger;
};

#endif //MAGICPLAYER_GUI_HPP
