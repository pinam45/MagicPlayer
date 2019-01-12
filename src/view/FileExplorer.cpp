//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "view/FileExplorer.hpp"
#include "view/imgui_helpers.hpp"
#include "utils/log.hpp"

#include <imgui.h>
#include <IconsFontAwesome5.h>

#include <sstream>

FileExplorer::FileExplorer(std::string name, Msg::Sender sender)
  : m_sender(sender)
  , m_name(std::move(name))
  , m_user_path()
  , m_path()
  , m_content()
  , m_formatted_content()
  , selected_content(std::numeric_limits<std::size_t>::max())
  , m_sub_paths()
  , m_displayable_split_paths(0)
  , m_last_available_width(0.0f)
  , m_text_field_focus_state(0)
  , m_logger(spdlog::get(VIEW_LOGGER_NAME))
{
}

void FileExplorer::init()
{
}

void FileExplorer::show()
{
	ImGui::Begin(m_name.c_str()); // Explorer

	// Display navigation buttons list
	if(m_text_field_focus_state == 0)
	{
		// Compute displayable split paths number
		const float available_width = ImGui::GetWindowContentRegionMax().x * 3 / 4;
		if(std::abs(m_last_available_width - available_width)
		   > std::numeric_limits<float>::epsilon())
		{
			m_last_available_width = available_width;
			float used_width = ImGui::GetWindowPos().x + compute_text_width(ICON_FA_CHEVRON_RIGHT);
			m_displayable_split_paths = 0;
			for(auto it = m_sub_paths.rbegin(); it != m_sub_paths.rend(); ++it)
			{
				used_width += compute_text_width(std::get<0>(*it).c_str());
				if(used_width > available_width)
				{
					break;
				}
				++m_displayable_split_paths;
			}
		}

		// Display hidden folders
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ChildBg]);
		if(m_displayable_split_paths < m_sub_paths.size())
		{
			if(ImGui::Button(ICON_FA_CHEVRON_RIGHT))
			{
				ImGui::OpenPopup("Hidden folders");
			}
			if(ImGui::BeginPopup("Hidden folders"))
			{
				for(std::size_t i = 0; i < m_sub_paths.size() - m_displayable_split_paths; ++i)
				{
					if(ImGui::MenuItem(std::get<0>(m_sub_paths[i]).c_str()))
					{
						m_sender.sendInMessage<Msg::In::Open>(std::get<1>(m_sub_paths[i]));
					}
				}
				ImGui::EndPopup();
			}
			ImGui::SameLine(0, 0);
		}

		// Display visible folders
		for(std::size_t i = m_sub_paths.size() - m_displayable_split_paths; i < m_sub_paths.size();
		    ++i)
		{
			if(ImGui::Button(std::get<0>(m_sub_paths[i]).c_str()))
			{
				m_sender.sendInMessage<Msg::In::Open>(std::get<1>(m_sub_paths[i]));
			}
			ImGui::SameLine(0, 0);
		}
		ImGui::PopStyleColor();
	}

	// Display path text input
	// ImGui::InputText content can't be changed while edited
	// So we change the "dummy user path" to the real "user path" on focus gain, in 3 steps / states
	ImGui::PushItemWidth(-1);
	if(m_text_field_focus_state == 2)
	{
		if(ImGui::InputText("##user path",
		                    m_user_path.data(),
		                    m_user_path.size(),
		                    ImGuiInputTextFlags_EnterReturnsTrue))
		{
			std::filesystem::path file_path;
			if(utf8_string_to_path(m_user_path.data(), file_path))
			{
				m_logger->info("Request to open {}", file_path);
				m_sender.sendInMessage<Msg::In::Open>(std::move(file_path));
			}
			else
			{
				m_logger->warn("Inputed path contain invalid utf8 characters: {}",
				               invalid_utf8_path_representation(m_user_path.data()));
			}
		}
		if(!ImGui::IsItemActive())
		{
			m_text_field_focus_state = 0;
			m_user_path[0] = '\0';
		}
	}
	else if(m_text_field_focus_state == 1)
	{
		std::string path;
		if(!path_to_generic_utf8_string(m_path, path))
		{
			path = path_to_generic_utf8_string(m_path);
			m_logger->warn("Current path contain invalid utf8 characters: {}",
			               invalid_utf8_path_representation(m_user_path.data()));
		}
		std::strncpy(m_user_path.data(), path.data(), m_user_path.size());
		m_user_path[std::max(path.size(), m_user_path.size() - 1)] = '\0';

		ImGui::SetKeyboardFocusHere();
		ImGui::InputText("##user path",
		                 m_user_path.data(),
		                 m_user_path.size(),
		                 ImGuiInputTextFlags_EnterReturnsTrue);
		++m_text_field_focus_state;
	}
	else if(m_text_field_focus_state == 0)
	{
		ImGui::InputText("##dummy user path", m_user_path.data(), m_user_path.size());
		if(m_text_field_focus_state == 0 && ImGui::IsItemActive())
		{
			++m_text_field_focus_state;
		}
	}
	ImGui::PopItemWidth();

	// Display folder content
	if(ImGui::BeginChild("Content"))
	{
		for(std::size_t i = 0; i < m_formatted_content.size(); ++i)
		{
			if(ImGui::Selectable(m_formatted_content[i].c_str(), (i == selected_content)))
			{
				selected_content = i;
			}
			if(ImGui::IsItemClicked() && ImGui::IsMouseDoubleClicked(0))
			{
				m_sender.sendInMessage<Msg::In::Open>(m_content[i].path);
			}
		}
	}
	ImGui::EndChild();
	if(ImGui::IsItemClicked())
	{
		selected_content = m_content.size();
	}

	ImGui::End(); // Explorer
}

void FileExplorer::processMessage(Msg::Out::FolderContent& message)
{
	std::error_code error;
	std::filesystem::path path = std::filesystem::canonical(message.path, error);
	if(error)
	{
		SPDLOG_DEBUG(m_logger, "std::filesystem::canonical failed: {}", error.message());
		m_logger->warn("Failed to get canonical path of folder {}", path);
		return;
	}

	std::string path_str;
	if(!path_to_generic_utf8_string(path, path_str))
	{
		m_logger->warn("Received content of folder with invalid utf8 path: {}",
		               invalid_utf8_path_representation(path));
		return;
	}

	// Generate sub paths
	std::vector<std::tuple<std::string, std::filesystem::path>> sub_paths;
	std::string sub_path;
	std::string path_part;
	std::stringstream path_stream(path_str);
	while(std::getline(path_stream, path_part, '/'))
	{
		if(path_part.empty())
		{
			// For Linux root folder
			sub_path.append("/");
			path_part = "Root";
		}
		else
		{
			sub_path.append(path_part);
			sub_path.append("/");
		}
		sub_paths.emplace_back(path_part, utf8_string_to_path(sub_path));
	}

	if(!sub_paths.empty())
	{
		for(std::size_t i = 0; i < sub_paths.size() - 1; ++i)
		{
			std::get<0>(sub_paths[i]) += " " ICON_FA_CHEVRON_RIGHT;
		}
	}

	// Apply modifications
	m_path = std::move(path);
	m_sub_paths = std::move(sub_paths);
	m_content = message.content;
	selected_content = m_content.size();

	m_displayable_split_paths = m_sub_paths.size();
	m_last_available_width = 0.0f;
	m_text_field_focus_state = 0;

	// Generate formatted content list
	m_formatted_content.clear();
	for(const PathInfo& info: m_content)
	{
		std::string icon;
		if(info.is_folder)
		{
			icon = ICON_FA_FOLDER;
		}
		else
		{
			if(info.has_supported_audio_extension)
			{
				icon = ICON_FA_FILE_AUDIO;
			}
			else
			{
				icon = ICON_FA_FILE;
			}
		}
		m_formatted_content.push_back(icon + " " + info.file_name);
	}
}
