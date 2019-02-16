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
#include "view/windows/Explorer.hpp"
#include "view/windows/Player.hpp"
#include "view/windows/LogViewer.hpp"
#include "view/windows/SettingsEditor.hpp"

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

	void loadInitialConfig();

	void setupImGui();
	void setupFonts();
	void setupStyle();

	void processMessages();

	Msg::Com& m_com;
	bool m_showThemeConfigWindow;
	bool m_showLogViewerWindow;
	bool m_showSettingsEditor;
	ImGui::ETheming::ColorTheme m_style;

	Explorer m_explorer;
	Player m_player;
	LogViewer m_log_viewer;
	SettingsEditor m_settingsEditor;

	std::shared_ptr<spdlog::logger> m_logger;
};

#endif //MAGICPLAYER_GUI_HPP
