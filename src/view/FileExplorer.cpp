//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "view/FileExplorer.hpp"
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
  , m_music_file_path()
  , m_path(DEFAULT_PATH)
  , m_content()
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

	// FIXME: temporary file path text filed
	constexpr char const* const music_path_label_text = "Music file path:";
	ImFont const* const font = ImGui::GetFont();
	ImVec2 text_size = font->CalcTextSizeA(font->FontSize, FLT_MAX, FLT_MAX, music_path_label_text);
	ImGui::PushItemWidth(text_size.x);
	ImGui::LabelText("##file_path_pre", "%s", music_path_label_text);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - text_size.x
	                     - 4 * ImGui::GetFontSize());
	bool try_open = ImGui::InputText("##file_path",
	                                 m_music_file_path.data(),
	                                 m_music_file_path.size(),
	                                 ImGuiInputTextFlags_EnterReturnsTrue);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	try_open |= ImGui::Button("open");

	if(try_open)
	{
		std::filesystem::path file_path(m_music_file_path.data());
		m_logger->info("Request to open {}", file_path);
		sendMessage<Msg::In::Open>(std::move(file_path));
	}

	if(ImGui::BeginChild("Content"))
	{
		for(const PathInfo& info: m_content)
		{
			if(info.is_folder)
			{
				ImGui::Text(ICON_FA_FOLDER " %s", info.file_name.c_str());
			}
			else
			{
				ImGui::Text(ICON_FA_FILE " %s", info.file_name.c_str());
			}
			if(ImGui::IsItemClicked() && ImGui::IsMouseDoubleClicked(0))
			{
				sendMessage<Msg::In::Open>(info.path);
			}
		}
	}
	ImGui::EndChild();

	ImGui::End(); // Explorer
}

void FileExplorer::processMessage(Msg::Out::FolderContent& message)
{
	m_path = message.path;
	m_content = message.content;
}
