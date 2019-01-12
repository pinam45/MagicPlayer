//
// Copyright (c) 2019 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_SETTINGSEDITOR_HPP
#define MAGICPLAYER_SETTINGSEDITOR_HPP

#include "model/Messages.hpp"
#include "data/Settings.hpp"

#include <spdlog/logger.h>

#include <memory>
#include <string>
#include <array>

class SettingsEditor
{
public:
	SettingsEditor(std::string name, Msg::Sender sender);

	void init();

	void show(bool& open);

	void processMessage(Msg::Out::Settings& message);

private:
	void showLeftPanel(float panel_height) noexcept;

	void showRightPanel(float panel_height) noexcept;

	void showConfigOptions() noexcept;

	void showFileExplorerConfigOptions() noexcept;

	void showMusicsDatabaseConfigOptions() noexcept;

	void showErrorPopupModal() noexcept;

	void revertSettingsInputs() noexcept;

	bool applySettingsInputs() noexcept;

	enum class SettingsPanels
	{
		FILES_EXPLORER,
		MUSICS_DATABASE
	};
	std::string_view SettingsPanelsTxt(SettingsPanels settingsPanels) const noexcept;

	Msg::Sender m_sender;

	std::string m_name;
	std::array<char, 2048> m_explorer_folder_buffer;
	std::vector<std::array<char, 2048>> m_musics_sources_buffers;
	data::Settings m_settings;
	SettingsPanels m_selectedPanel;
	std::string m_error_message;

	std::shared_ptr<spdlog::logger> m_logger;
};

#endif //MAGICPLAYER_SETTINGSEDITOR_HPP
