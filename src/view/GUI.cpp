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
	constexpr unsigned int WINDOW_INITIAL_WIDTH = 600;
	constexpr unsigned int WINDOW_INITIAL_HEIGHT = 400;
	constexpr float MUSIC_INITIAL_VOLUME = 20.0f;
	constexpr float MUSIC_OFFSET_REFRESH_SECONDS = 0.1f;
	constexpr unsigned int FRAME_RATE_LIMIT = 60;
	constexpr const char* DROID_SANS_MONO_FONT_PATH = "resources/fonts/DroidSans/DroidSansMono.ttf";
	constexpr const char* FONTAWESOME_FONT_PATH =
	  "resources/fonts/fontawesome-free-5.4.0/" FONT_ICON_FILE_NAME_FAS;
	constexpr const char* DEFAULT_FONT_PATH = DROID_SANS_MONO_FONT_PATH;
	constexpr float DEFAULT_FONT_SIZE = 13.5f;
	constexpr float LARGE_FONT_SIZE = 22.0f;
	constexpr const char* INNER_WINDOW_MAIN_NAME = "Main";
	constexpr const char* MAIN_DOCKSPACE_NAME = "Main dockspace";
	constexpr const char* INNER_WINDOW_PLAYER_NAME = "Player";
	constexpr const char* INNER_WINDOW_EXPLORER_NAME = "Explorer";
	constexpr const char* INNER_WINDOW_LOG_VIEWER_NAME = "Log viewer";
} // namespace

template<typename Message, typename... Args>
void GUI::sendMessage(Args&&... args)
{
	m_com.in.emplace_back(std::in_place_type_t<Message>{}, std::forward<Args>(args)...);
}

template<>
void GUI::handleMessage(Msg::Out::MusicOffset& message)
{
	m_musicInfos.offset = message.seconds;
}

template<>
void GUI::handleMessage(Msg::Out::MusicInfo& message)
{
	m_musicInfos.valid = message.valid;
	m_musicInfos.offset = 0; //FIXME?
	m_musicInfos.duration = message.durationSeconds;
	m_logger->info("Received music information: valid = {}, duration = {:.2f} seconds",
	               m_musicInfos.valid,
	               m_musicInfos.duration);
}

template<>
void GUI::handleMessage(Msg::Out::FolderContent& message)
{
	m_logger->info(
	  "Received folder {} content: {} files/folders", message.path, message.content.size());
	m_file_explorer.processMessage(message);
}

GUI::GUI(Msg::Com& com_)
  : m_musicInfos()
  , m_com(com_)
  , m_showThemeConfigWindow(false)
  , m_showLogViewerWindow(false)
  , m_volume(MUSIC_INITIAL_VOLUME)
  , m_style(ImGui::ETheming::ColorTheme::ArcDark)
  , m_normal_font(nullptr)
  , m_large_font(nullptr)
  , m_file_explorer(INNER_WINDOW_EXPLORER_NAME, m_com.in)
  , m_log_viewer(INNER_WINDOW_LOG_VIEWER_NAME)
  , m_logger(spdlog::get(VIEW_LOGGER_NAME))
{
}

int GUI::run()
{
	sf::Clock deltaClock;
	sf::Clock musicOffsetClock;

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

		// Request music offset
		if(m_musicInfos.valid
		   && musicOffsetClock.getElapsedTime() > sf::seconds(MUSIC_OFFSET_REFRESH_SECONDS))
		{
			sendMessage<Msg::In::RequestMusicOffset>();
			musicOffsetClock.restart();
		}

		ImGui::SFML::Update(window, deltaClock.restart());

		show();

		window.clear();
		ImGui::SFML::Render(window);
		window.display();
	}
	SPDLOG_DEBUG(m_logger, "Main loop ended");

	sendMessage<Msg::In::Control>(Msg::In::Control::Action::STOP);
	sendMessage<Msg::In::Close>();

	ImGui::SFML::Shutdown();
	SPDLOG_DEBUG(m_logger, "imgui-SFML shutdown");
	return EXIT_SUCCESS;
}

void GUI::show()
{
	showMainDockspace();
	showPlayer();
	m_file_explorer.show();

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
		if(ImGui::BeginMenu("View"))
		{
			if(ImGui::MenuItem("Theme configuration", nullptr, &m_showThemeConfigWindow))
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
			if(ImGui::MenuItem("Logs console", nullptr, &m_showLogViewerWindow))
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
		  ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.10f, nullptr, &dock_main_id);

		ImGui::DockBuilderDockWindow(INNER_WINDOW_EXPLORER_NAME, dock_main_id);
		ImGui::DockBuilderDockWindow(INNER_WINDOW_PLAYER_NAME, dock_id_bottom);
		ImGui::DockBuilderFinish(dockspace_id);
		SPDLOG_DEBUG(m_logger, "Set initial position of windows in the main dockspace");
	}

	ImGui::DockSpace(dockspace_id);

	ImGui::End();
}

void GUI::showPlayer()
{
	ImGui::Begin(INNER_WINDOW_PLAYER_NAME);
	ImGui::PushFont(m_large_font);

	if(m_musicInfos.valid)
	{
		// Control buttons
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4());
		if(ImGui::Button(ICON_FA_PLAY))
		{
			m_logger->info("Request to play/resume music");
			sendMessage<Msg::In::Control>(Msg::In::Control::Action::PLAY);
		}
		ImGui::SameLine();
		if(ImGui::Button(ICON_FA_PAUSE))
		{
			m_logger->info("Request to pause music");
			sendMessage<Msg::In::Control>(Msg::In::Control::Action::PAUSE);
		}
		ImGui::SameLine();
		if(ImGui::Button(ICON_FA_STOP))
		{
			m_logger->info("Request to stop music");
			sendMessage<Msg::In::Control>(Msg::In::Control::Action::STOP);
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
			sendMessage<Msg::In::MusicOffset>(trac_pos);
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
		sendMessage<Msg::In::Volume>(false, m_volume);
	}
	ImGui::PopItemWidth();

	ImGui::PopFont();
	ImGui::End();
}

ImFont* GUI::loadFonts(float pixel_size)
{
	ImGuiIO& io = ImGui::GetIO();

	static constexpr ImWchar font_ranges[] = {0x0001, 0xFFFF, 0};
	ImFont* default_font =
	  io.Fonts->AddFontFromFileTTF(DEFAULT_FONT_PATH, pixel_size, nullptr, font_ranges);
	if(default_font)
	{
		SPDLOG_DEBUG(m_logger, "Loaded font {} {:.2f}px", DEFAULT_FONT_PATH, pixel_size);
	}
	else
	{
		io.Fonts->AddFontDefault();
		m_logger->warn("Failed to load font {}: use default font instead", DEFAULT_FONT_PATH);
	}

	static constexpr ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.GlyphMinAdvanceX = pixel_size;
	ImFont* font =
	  io.Fonts->AddFontFromFileTTF(FONTAWESOME_FONT_PATH, pixel_size, &icons_config, icons_ranges);
	if(font)
	{
		SPDLOG_DEBUG(m_logger, "Loaded font {} {:.2f}px", FONTAWESOME_FONT_PATH, pixel_size);
	}
	else
	{
		m_logger->warn("Failed to load fontawesome ({}): icons disabled", FONTAWESOME_FONT_PATH);
		font = default_font;
	}

	return font;
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
	m_normal_font = loadFonts(DEFAULT_FONT_SIZE);
	m_large_font = loadFonts(LARGE_FONT_SIZE);
	ImGui::SFML::UpdateFontTexture();
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

	m_showThemeConfigWindow = false;
	m_volume = MUSIC_INITIAL_VOLUME;
	m_style = ImGui::ETheming::ColorTheme::ArcDark;

	m_musicInfos.valid = false;
	m_musicInfos.offset = 0;
	m_musicInfos.duration = 0;

	m_file_explorer.init();
	m_log_viewer.init();
	for(const char* logger_name: LOGGERS_NAMES)
	{
		auto logger = spdlog::get(logger_name);
		if(logger)
		{
			m_log_viewer.watch_logger(logger);
		}
	}

	sendMessage<Msg::In::Volume>(false, m_volume);
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
