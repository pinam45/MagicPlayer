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

namespace
{
	constexpr const char* DEFAULT_PATH = "./";
} // namespace

template<typename Message, typename... Args>
void FileExplorer::sendMessage(Args&&... args)
{
	m_com_in.emplace_back(std::in_place_type_t<Message>{}, std::forward<Args>(args)...);
}

FileExplorer::FileExplorer(std::string name, shared_queue<Msg::Com::InMessage>& com_in)
  : m_com_in(com_in)
  , m_name(std::move(name))
  , m_user_path()
  , m_path(DEFAULT_PATH)
  , m_content()
  , m_formatted_content()
  , selected_content(std::numeric_limits<std::size_t>::max())
  , m_split_path()
  , m_sub_paths()
  , m_displayable_split_paths(0)
  , m_last_available_width(0.0f)
  , m_text_field_focus_state(0)
  , m_logger(spdlog::get(VIEW_LOGGER_NAME))
{
}

void FileExplorer::init()
{
	sendMessage<Msg::In::Open>(m_path);
}

void FileExplorer::show()
{
	ImGui::Begin(m_name.c_str());

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
			for(auto it = m_split_path.rbegin(); it != m_split_path.rend(); ++it)
			{
				used_width += compute_text_width(it->c_str());
				if(used_width > available_width)
				{
					break;
				}
				++m_displayable_split_paths;
			}
		}

		// Display hidden folders
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ChildBg]);
		if(m_displayable_split_paths < m_split_path.size())
		{
			if(ImGui::Button(ICON_FA_CHEVRON_RIGHT))
			{
				ImGui::OpenPopup("Hidden folders");
			}
			if(ImGui::BeginPopup("Hidden folders"))
			{
				for(std::size_t i = 0; i < m_split_path.size() - m_displayable_split_paths; ++i)
				{
					if(ImGui::MenuItem(m_split_path[i].c_str()))
					{
						sendMessage<Msg::In::Open>(m_sub_paths[i]);
					}
				}
				ImGui::EndPopup();
			}
			ImGui::SameLine(0, 0);
		}

		// Display visible folders
		for(std::size_t i = m_split_path.size() - m_displayable_split_paths;
		    i < m_split_path.size();
		    ++i)
		{
			if(ImGui::Button(m_split_path[i].c_str()))
			{
				sendMessage<Msg::In::Open>(m_sub_paths[i]);
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
			std::filesystem::path file_path(m_user_path.data());
			m_logger->info("Request to open {}", file_path);
			sendMessage<Msg::In::Open>(std::move(file_path));
		}
		if(!ImGui::IsItemActive())
		{
			m_text_field_focus_state = 0;
			m_user_path[0] = '\0';
		}
	}
	else if(m_text_field_focus_state == 1)
	{
		std::string path = m_path.generic_string();
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
				sendMessage<Msg::In::Open>(m_content[i].path);
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
	m_path = std::filesystem::canonical(message.path);
	m_content = message.content;

	// Generate formatted content list
	m_formatted_content.clear();
	for(const PathInfo& info: m_content)
	{
		if(info.is_folder)
		{
			m_formatted_content.push_back(std::string(ICON_FA_FOLDER) + " " + info.file_name);
		}
		else
		{
			m_formatted_content.push_back(std::string(ICON_FA_FILE) + " " + info.file_name);
		}
	}
	selected_content = m_content.size();

	// Generate split paths and sub-paths
	m_split_path.clear();
	m_sub_paths.clear();
	std::stringstream path_stream(m_path.generic_string());
	std::string path_part;
	std::filesystem::path sub_path;
	while(std::getline(path_stream, path_part, '/'))
	{
		if(path_part.empty())
		{
			sub_path.append("/");
			path_part = "Root";
		}
		else
		{
			sub_path.append(path_part);
		}
		m_split_path.push_back(path_part);
		m_sub_paths.push_back(sub_path);
	}

	for(std::size_t i = 0; i < m_split_path.size() - 1; ++i)
	{
		m_split_path[i] += " " ICON_FA_CHEVRON_RIGHT;
	}

	// Reset states
	m_displayable_split_paths = m_split_path.size();
	m_last_available_width = 0.0f;
	m_text_field_focus_state = 0;
}
