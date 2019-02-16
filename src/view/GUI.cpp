//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "view/GUI.hpp"
#include "view/imgui_easy_theming.hpp"
#include "view/imgui_custom_widgets.hpp"
#include "view/FontManager.hpp"
#include "utils/log.hpp"

#include <IconsFontAwesome5.h>
#include <imgui.h>
#include <imgui_internal.h> //FIXME
#include <imgui-SFML.h>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <spdlog/spdlog.h>

namespace
{
	constexpr const char* WINDOW_NAME = "MagicPlayer";
	constexpr unsigned int WINDOW_INITIAL_WIDTH = 900;
	constexpr unsigned int WINDOW_INITIAL_HEIGHT = 600;
	constexpr unsigned int FRAME_RATE_LIMIT = 60;

	constexpr const char* INNER_WINDOW_MAIN_NAME = "Main";
	constexpr const char* MAIN_DOCKSPACE_NAME = "Main dockspace";
	constexpr const char* INNER_WINDOW_PLAYER_NAME = "Player";
	constexpr const char* INNER_WINDOW_EXPLORER_NAME = "Explorer";
	constexpr const char* INNER_WINDOW_LOG_VIEWER_NAME = "Log viewer";
	constexpr const char* INNER_WINDOW_SETTINGS_EDITOR_NAME = "Settings";

	constexpr const char* MENU_SETTING_TXT = ICON_FA_WRENCH " Settings";
	constexpr const char* MENU_THEME_TXT = ICON_FA_PAINT_BRUSH " Theme";
	constexpr const char* MENU_LOG_TXT = ICON_FA_CLIPBOARD_LIST " Logs";
} // namespace

template<>
void GUI::handleMessage(Msg::Out::MusicOffset& message)
{
	m_player.processMessage(message);
}

template<>
void GUI::handleMessage(Msg::Out::MusicInfo& message)
{
	m_logger->info("Received music information: valid = {}, duration = {:.2f} seconds",
	               message.valid,
	               message.durationSeconds);
	m_player.processMessage(message);
}

template<>
void GUI::handleMessage(Msg::Out::FolderContent& message)
{
	m_logger->info(
	  "Received folder {} content: {} files/folders", message.path, message.content.size());
	m_explorer.processMessage(message);
}

template<>
void GUI::handleMessage(Msg::Out::Database& message)
{
	assert(message.database != nullptr);
	m_logger->info("Received database");
	m_settingsEditor.processMessage(message);
}

template<>
void GUI::handleMessage(Msg::Out::Settings& message)
{
	m_logger->info("Received settings");
	m_settingsEditor.processMessage(message);
}

GUI::GUI(Msg::Com& com_)
  : m_com(com_)
  , m_showThemeConfigWindow(false)
  , m_showLogViewerWindow(false)
  , m_showSettingsEditor(false)
  , m_style(ImGui::ETheming::ColorTheme::ArcDark)
  , m_explorer(INNER_WINDOW_EXPLORER_NAME, Msg::Sender(m_com))
  , m_player(INNER_WINDOW_PLAYER_NAME, Msg::Sender(m_com))
  , m_log_viewer(INNER_WINDOW_LOG_VIEWER_NAME)
  , m_settingsEditor(INNER_WINDOW_SETTINGS_EDITOR_NAME, Msg::Sender(m_com))
  , m_logger(spdlog::get(VIEW_LOGGER_NAME))
{
}

int GUI::run()
{
	sf::Clock deltaClock;

	sf::RenderWindow window(sf::VideoMode(WINDOW_INITIAL_WIDTH, WINDOW_INITIAL_HEIGHT),
	                        WINDOW_NAME);
	window.setFramerateLimit(FRAME_RATE_LIMIT);
	ImGui::SFML::Init(window, false);

	loadInitialConfig();
	setupImGui();
	setupFonts();
	setupStyle();

	while(window.isOpen())
	{
		sf::Event event{};
		while(window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			if(event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		// Process messages
		processMessages();

		ImGui::SFML::Update(window, deltaClock.restart());

		show();

		window.clear();
		ImGui::SFML::Render(window);
		window.display();
	}
	SPDLOG_DEBUG(m_logger, "Main loop ended");

	m_com.sendInMessage<Msg::In::Control>(Msg::In::Control::Action::STOP);
	m_com.sendInMessage<Msg::In::Close>();

	ImGui::SFML::Shutdown();
	SPDLOG_DEBUG(m_logger, "imgui-SFML shutdown");
	return EXIT_SUCCESS;
}

void GUI::show()
{
	showMainDockspace();
	m_explorer.show();
	m_player.show();

	if(m_showThemeConfigWindow)
	{
		ImGui::ETheming::showThemeConfigWindow(&m_style, &m_showThemeConfigWindow);
		if(!m_showThemeConfigWindow)
		{
			SPDLOG_DEBUG(m_logger, "Close theme configuration window");
		}
	}
	if(m_showLogViewerWindow)
	{
		m_log_viewer.show(m_showLogViewerWindow);
		if(!m_showLogViewerWindow)
		{
			SPDLOG_DEBUG(m_logger, "Close logs console");
		}
	}
	if(m_showSettingsEditor)
	{
		m_settingsEditor.show(m_showSettingsEditor);
		if(!m_showSettingsEditor)
		{
			SPDLOG_DEBUG(m_logger, "Close settings editor");
		}
	}
}

void GUI::showMainDockspace()
{
	// Open "full-windowed" imgui window
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin(INNER_WINDOW_MAIN_NAME,
	             nullptr,
	             ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
	               | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings
	               | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
	               | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);
	ImGui::PopStyleVar(2);

	// Show menu bar
	if(ImGui::BeginMenuBar())
	{
		if(ImGui::BeginMenu("Edit"))
		{
			if(ImGui::MenuItem(MENU_SETTING_TXT, nullptr, &m_showSettingsEditor))
			{
				if(m_showSettingsEditor)
				{
					SPDLOG_DEBUG(m_logger, "Show settings editor window");
				}
				else
				{
					SPDLOG_DEBUG(m_logger, "Hide settings editor window");
				}
			}
			if(ImGui::MenuItem(MENU_THEME_TXT, nullptr, &m_showThemeConfigWindow))
			{
				if(m_showThemeConfigWindow)
				{
					SPDLOG_DEBUG(m_logger, "Show theme configuration window");
				}
				else
				{
					SPDLOG_DEBUG(m_logger, "Hide theme configuration window");
				}
			}
			ImGui::EndMenu();
		}
		if(ImGui::BeginMenu("View"))
		{
			if(ImGui::MenuItem(MENU_LOG_TXT, nullptr, &m_showLogViewerWindow))
			{
				if(m_showLogViewerWindow)
				{
					SPDLOG_DEBUG(m_logger, "Show logs console");
				}
				else
				{
					SPDLOG_DEBUG(m_logger, "Hide logs console");
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGuiID dockspace_id = ImGui::GetID(MAIN_DOCKSPACE_NAME);
	if(ImGui::DockBuilderGetNode(dockspace_id) == nullptr)
	{
		// Main dockspace initial setup
		ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
		ImGui::DockBuilderAddNode(dockspace_id, ImGui::GetMainViewport()->Size); // Add empty node

		ImGuiID dock_main_id = dockspace_id;
		ImGuiID dock_id_bottom =
		  ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.12f, nullptr, &dock_main_id);

		ImGui::DockBuilderDockWindow(INNER_WINDOW_EXPLORER_NAME, dock_main_id);
		ImGui::DockBuilderDockWindow(INNER_WINDOW_PLAYER_NAME, dock_id_bottom);
		ImGui::DockBuilderFinish(dockspace_id);
		SPDLOG_DEBUG(m_logger, "Set initial position of windows in the main dockspace");
	}

	ImGui::DockSpace(dockspace_id);

	ImGui::End();
}

void GUI::setupImGui()
{
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable docking
	io.ConfigDockingWithShift = true; // hold shift to use docking
	io.IniFilename = nullptr; // disable .ini saving
	SPDLOG_DEBUG(m_logger, "Setup ImGui");
}

void GUI::setupFonts()
{
	FontManager::loadFontSize(FontManager::DEFAULT_FONT_SIZE);
	FontManager::loadFontSize(FontManager::LARGE_FONT_SIZE);
	SPDLOG_DEBUG(m_logger, "Setup fonts");
}

void GUI::setupStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 0.0f;
	style.ScrollbarRounding = 0.0f;
	ImGui::ETheming::setColorTheme(m_style);
	SPDLOG_DEBUG(m_logger, "Setup style");
}

void GUI::loadInitialConfig()
{
	// FIXME: load/save config from file
	m_style = ImGui::ETheming::ColorTheme::MintYDark;

	m_explorer.init();
	m_player.init();
	m_log_viewer.init();
	m_settingsEditor.init();

	SPDLOG_DEBUG(m_logger, "Sent initial config messages");
}

void GUI::processMessages()
{
	while(!m_com.out.empty())
	{
		Msg::Com::OutMessage message_ = m_com.out.front();
		std::visit(
		  [&](auto&& message) noexcept {
			  SPDLOG_TRACE(m_logger, "Received message {}", message);
			  handleMessage(message);
		  },
		  message_);
		m_com.out.pop_front();
	}
}
