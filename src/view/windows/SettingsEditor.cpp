//
// Copyright (c) 2019 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "view/windows/SettingsEditor.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <IconsFontAwesome5.h>

#include <sstream>

namespace
{
	constexpr int DEFAULT_WIDTH = 600;
	constexpr int DEFAULT_HEIGHT = 450;
	constexpr int LEFT_PANEL_WIDTH = 150;
	constexpr float BUTTONS_SPACING_FACTOR = 2.5f;
	constexpr const char* LEFT_PANEL_TITLE_TXT = ICON_FA_COGS " Categories";
	constexpr const char* REVERT_BUTTON_TXT = ICON_FA_UNDO " Revert";
	constexpr const char* SAVE_BUTTON_TXT = ICON_FA_SAVE " Save";
	constexpr const char* DELETE_MUSIC_SOURCE_BUTTON_TXT = ICON_FA_TRASH_ALT;
	constexpr const char* ADD_MUSIC_SOURCE_BUTTON_TXT = ICON_FA_FOLDER_PLUS;
	constexpr const char* DEFAULT_GENERATION_DATE_TXT = "undefined";
	const ImVec2 ERROR_POPUP_SIZE = {375.f, -1.f};
} // namespace

SettingsEditor::SettingsEditor(std::string name, Msg::Sender sender)
  : m_sender(sender)
  , m_name(std::move(name))
  , m_explorer_folder_buffer()
  , m_musics_sources_buffers()
  , m_settings()
  , m_database_info()
  , m_selectedPanel(SettingsPanels::FILES_EXPLORER)
  , m_error_message("")
  , m_logger(spdlog::get(VIEW_LOGGER_NAME))
{
	m_explorer_folder_buffer[0] = '\0';
}

void SettingsEditor::init()
{
}

void SettingsEditor::show(bool& open)
{
	ImGui::SetNextWindowSize(ImVec2(DEFAULT_WIDTH, DEFAULT_HEIGHT), ImGuiCond_Once);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if(!ImGui::Begin(m_name.c_str(), &open))
	{
		ImGui::PopStyleVar();
		ImGui::End();
		return;
	}
	ImGui::PopStyleVar();

	const ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;

	// Leave room for buttons
	const float panels_height = -(ImGui::GetFrameHeightWithSpacing() + style.ItemSpacing.y);

	// Left panel
	showLeftPanel(panels_height);

	// Panels separator
	ImGui::SameLine(0.f, 0.f);
	ImGui::VerticalSeparator();
	ImGui::SameLine(0.f, 0.f);

	// Right panel
	showRightPanel(panels_height);

	// Buttons separator
	ImGui::Separator();

	// Buttons
	// Align right
	ImGui::SetCursorPosX(
	  ImGui::GetCursorPos().x
	  + (ImGui::GetContentRegionAvailWidth()
	     - (ImGui::CalcTextSize(REVERT_BUTTON_TXT).x + ImGui::CalcTextSize(SAVE_BUTTON_TXT).x
	        + 4.0f * style.FramePadding.x + 2.f * BUTTONS_SPACING_FACTOR * style.ItemSpacing.x)));
	if(ImGui::Button(REVERT_BUTTON_TXT))
	{
		SPDLOG_DEBUG(m_logger, "Revert settings");
		revertSettingsInputs();
	}
	ImGui::SameLine(0.f, BUTTONS_SPACING_FACTOR * style.ItemSpacing.x);
	if(ImGui::Button(SAVE_BUTTON_TXT))
	{
		SPDLOG_DEBUG(m_logger, "Save settings");
		if(applySettingsInputs())
		{
			m_sender.sendInMessage<Msg::In::Settings>(m_settings, true);
		}
		else
		{
			ImGui::OpenPopup("Error##SettingsEditor");
		}
	}
	showErrorPopupModal();

	ImGui::End();
}

void SettingsEditor::processMessage(Msg::Out::Settings& message)
{
	SPDLOG_DEBUG(m_logger, "Received settings");
	m_settings = message.settings;
	revertSettingsInputs();
}

void SettingsEditor::processMessage(Msg::Out::Database& message)
{
	assert(message.database != nullptr);
	std::time_t time_c = std::chrono::system_clock::to_time_t(message.database->generation_date);
	std::stringstream generation_date;
	generation_date << std::put_time(std::localtime(&time_c), "%F %T");
	m_database_info.generation_date = generation_date.str();
	m_database_info.sources = message.database->sources;
	m_database_info.artists = static_cast<int>(message.database->artists.size());
	m_database_info.albums = 0;
	m_database_info.musics = 0;
	for(const data::Artist& artist: message.database->artists)
	{
		m_database_info.albums += static_cast<int>(artist.albums.size());
		for(const data::Album& album: artist.albums)
		{
			m_database_info.musics += static_cast<int>(album.musics.size());
		}
	}
}

SettingsEditor::DatabaseInfo::DatabaseInfo() noexcept
  : generation_date(DEFAULT_GENERATION_DATE_TXT), sources(), artists(), albums(), musics()
{
}

void SettingsEditor::showLeftPanel(const float panel_height) noexcept
{
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	ImGui::BeginChild("##left panel", ImVec2(LEFT_PANEL_WIDTH, panel_height), true);
	ImGui::PopStyleVar();
	{
		ImGui::TextUnformatted(LEFT_PANEL_TITLE_TXT);
		ImGui::Separator();
		if(ImGui::Selectable(SettingsPanelsTxt(SettingsPanels::FILES_EXPLORER).data(),
		                     m_selectedPanel == SettingsPanels::FILES_EXPLORER))
		{
			m_selectedPanel = SettingsPanels::FILES_EXPLORER;
		}
		if(ImGui::Selectable(SettingsPanelsTxt(SettingsPanels::MUSICS_SOURCES).data(),
		                     m_selectedPanel == SettingsPanels::MUSICS_SOURCES))
		{
			m_selectedPanel = SettingsPanels::MUSICS_SOURCES;
		}
		if(ImGui::Selectable(SettingsPanelsTxt(SettingsPanels::DATABASE).data(),
		                     m_selectedPanel == SettingsPanels::DATABASE))
		{
			m_selectedPanel = SettingsPanels::DATABASE;
		}
	}
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	ImGui::EndChild();
	ImGui::PopStyleVar();
}

void SettingsEditor::showRightPanel(const float panel_height) noexcept
{
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	ImGui::BeginChild("##right panel", ImVec2(0, panel_height), true);
	ImGui::PopStyleVar();
	{
		showConfigOptions();
	}
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	ImGui::EndChild();
	ImGui::PopStyleVar();
}

void SettingsEditor::showConfigOptions() noexcept
{
	ImGui::TextUnformatted(SettingsPanelsTxt(m_selectedPanel).data());
	ImGui::Separator();
	switch(m_selectedPanel)
	{
		case SettingsPanels::FILES_EXPLORER:
			showFileExplorerConfigOptions();
			break;
		case SettingsPanels::MUSICS_SOURCES:
			showMusicsSourcesConfigOptions();
			break;
		case SettingsPanels::DATABASE:
			showDatabaseConfigOptions();
			break;
	}
}

void SettingsEditor::showFileExplorerConfigOptions() noexcept
{
	if(ImGui::TreeNodeEx("Default folder", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::PushItemWidth(-1);
		ImGui::InputText(
		  "##Default folder", m_explorer_folder_buffer.data(), m_explorer_folder_buffer.size());
		ImGui::PopItemWidth();
		ImGui::TreePop();
	}
}

void SettingsEditor::showMusicsSourcesConfigOptions() noexcept
{
	int to_delete = -1;
	const ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	if(ImGui::TreeNodeEx("Folders containing musics", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::PushItemWidth(-(ImGui::CalcTextSize(DELETE_MUSIC_SOURCE_BUTTON_TXT).x
		                       + 2.0f * style.FramePadding.x + style.ItemSpacing.x));
		int i = 0;
		for(auto& buffer: m_musics_sources_buffers)
		{
			ImGui::PushID(i);
			ImGui::InputText("##music folder", buffer.data(), buffer.size());
			ImGui::SameLine();
			if(ImGui::Button(DELETE_MUSIC_SOURCE_BUTTON_TXT))
			{
				to_delete = i;
			}
			ImGui::PopID();
			++i;
		}
		ImGui::PopItemWidth();
		if(ImGui::Button(ADD_MUSIC_SOURCE_BUTTON_TXT))
		{
			m_musics_sources_buffers.emplace_back();
			m_musics_sources_buffers.back()[0] = '\0';
		}
		ImGui::TreePop();
	}

	if(to_delete >= 0 && static_cast<size_t>(to_delete) < m_explorer_folder_buffer.size())
	{
		auto it = m_musics_sources_buffers.begin();
		std::advance(it, to_delete);
		m_musics_sources_buffers.erase(it);
	}
}

void SettingsEditor::showDatabaseConfigOptions() noexcept
{
	if(ImGui::TreeNodeEx(ICON_FA_CLOCK " Generation date", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::TextUnformatted(m_database_info.generation_date.c_str());
		ImGui::TreePop();
	}
	if(ImGui::TreeNodeEx(ICON_FA_FOLDER " Sources used", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for(const utf8_path& source: m_database_info.sources)
		{
			ImGui::TextUnformatted(source.str().data());
		}
		ImGui::TreePop();
	}
	if(ImGui::TreeNodeEx(ICON_FA_MUSIC " Content", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("%d artists", m_database_info.artists);
		ImGui::Text("%d albums", m_database_info.albums);
		ImGui::Text("%d musics", m_database_info.musics);
		ImGui::TreePop();
	}

	ImGui::Separator();
	if(ImGui::Button(ICON_FA_DATABASE " Generate new database"))
	{
		m_sender.sendInMessage<Msg::In::RequestDatabase>(true);
	}
}

void SettingsEditor::showErrorPopupModal() noexcept
{
	ImGui::SetNextWindowSize(ERROR_POPUP_SIZE);
	if(ImGui::BeginPopupModal("Error##SettingsEditor",
	                          nullptr,
	                          ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::TextWrapped("%s", m_error_message.c_str());
		ImGui::Separator();
		ImGui::SetItemDefaultFocus();
		if(ImGui::Button("OK"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void SettingsEditor::revertSettingsInputs() noexcept
{
	std::string_view explorer_folder_txt;
	if(!m_settings.explorer_folder.valid_encoding())
	{
		m_logger->warn("Settings path contain invalid utf8 characters: {}",
		               m_settings.explorer_folder);
		explorer_folder_txt = "[invalid]";
	}
	else
	{
		explorer_folder_txt = m_settings.explorer_folder.str();
	}
	std::strncpy(
	  m_explorer_folder_buffer.data(), explorer_folder_txt.data(), m_explorer_folder_buffer.size());
	m_explorer_folder_buffer[std::max(explorer_folder_txt.size(),
	                                  m_explorer_folder_buffer.size() - 1)] = '\0';

	m_musics_sources_buffers.clear();
	m_musics_sources_buffers.reserve(m_settings.music_sources.size());
	for(const utf8_path& path: m_settings.music_sources)
	{
		std::string_view path_txt;
		if(!path.valid_encoding())
		{
			m_logger->warn("Settings path contain invalid utf8 characters: {}",
			               m_settings.explorer_folder);
			path_txt = "[invalid]";
		}
		else
		{
			path_txt = path.str();
		}
		m_musics_sources_buffers.emplace_back();
		auto& path_buffer = m_musics_sources_buffers.back();
		std::strncpy(path_buffer.data(), path_txt.data(), path_buffer.size());
		path_buffer[std::max(path_txt.size(), path_buffer.size() - 1)] = '\0';
	}
}

bool SettingsEditor::applySettingsInputs() noexcept
{
	utf8_path explorer_folder = m_explorer_folder_buffer.data();
	if(!explorer_folder.valid_encoding())
	{
		std::stringstream tmp;
		tmp << "Explorer folder path contain invalid UTF8 characters:\n";
		tmp << m_explorer_folder_buffer.data();
		m_error_message = tmp.str();
		return false;
	}

	std::vector<utf8_path> music_sources;
	for(const auto& buffer: m_musics_sources_buffers)
	{
		utf8_path music_source = buffer.data();
		if(!music_source.valid_encoding())
		{
			std::stringstream tmp;
			tmp << "Music source path contain invalid UTF8 characters:\n";
			tmp << buffer.data();
			m_error_message = tmp.str();
			return false;
		}
		music_sources.push_back(std::move(music_source));
	}

	m_settings.explorer_folder = std::move(explorer_folder);
	m_settings.music_sources = std::move(music_sources);

	return true;
}

std::string_view SettingsEditor::SettingsPanelsTxt(
  SettingsEditor::SettingsPanels settingsPanels) const noexcept
{
	switch(settingsPanels)
	{
		case SettingsPanels::FILES_EXPLORER:
			return "Files explorer";
		case SettingsPanels::MUSICS_SOURCES:
			return "Musics sources";
		case SettingsPanels::DATABASE:
			return "Database";
	}
}
