//
// Copyright (c) 2019 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "view/windows/Explorer.hpp"
#include "utils/log.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <IconsFontAwesome5.h>

namespace
{
	constexpr int LEFT_PANEL_WIDTH = 150;
	constexpr const char* LEFT_PANEL_TITLE_TXT = ICON_FA_STREAM " Views";
} // namespace

Explorer::Explorer(std::string name, Msg::Sender sender)
  : m_sender(sender)
  , m_name(std::move(name))
  , m_selectedView(ExplorerViews::ARTISTS)
  , m_fileExplorer(sender)
  , m_logger(spdlog::get(VIEW_LOGGER_NAME))
{
}

void Explorer::init()
{
	m_fileExplorer.init();
}

void Explorer::show()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if(!ImGui::Begin(m_name.c_str()))
	{
		ImGui::PopStyleVar();
		ImGui::End();
		return;
	}
	ImGui::PopStyleVar();

	// Left panel
	showLeftPanel();

	// Panels separator
	ImGui::SameLine(0.f, 0.f);
	ImGui::VerticalSeparator();
	ImGui::SameLine(0.f, 0.f);

	// Right panel
	showRightPanel();

	ImGui::End();
}

void Explorer::processMessage(Msg::Out::FolderContent& message)
{
	m_fileExplorer.processMessage(message);
}

void Explorer::processMessage(Msg::Out::Database& message)
{
	//TODO
}

void Explorer::showLeftPanel() noexcept
{
	ImGui::BeginChild("##left panel", ImVec2(LEFT_PANEL_WIDTH, 0), true, ImGuiWindowFlags_NoBackground);
	{
		ImGui::TextUnformatted(LEFT_PANEL_TITLE_TXT);
		ImGui::Separator();
		if(ImGui::Selectable(ExplorerViewsTxt(ExplorerViews::ARTISTS).data(),
		                     m_selectedView == ExplorerViews::ARTISTS))
		{
			m_selectedView = ExplorerViews::ARTISTS;
		}
		if(ImGui::Selectable(ExplorerViewsTxt(ExplorerViews::ALBUMS).data(),
		                     m_selectedView == ExplorerViews::ALBUMS))
		{
			m_selectedView = ExplorerViews::ALBUMS;
		}
		if(ImGui::Selectable(ExplorerViewsTxt(ExplorerViews::MUSICS).data(),
		                     m_selectedView == ExplorerViews::MUSICS))
		{
			m_selectedView = ExplorerViews::MUSICS;
		}
		if(ImGui::Selectable(ExplorerViewsTxt(ExplorerViews::FILES).data(),
		                     m_selectedView == ExplorerViews::FILES))
		{
			m_selectedView = ExplorerViews::FILES;
		}
	}
	ImGui::EndChild();
}

void Explorer::showRightPanel() noexcept
{
	ImGui::BeginChild("##right panel", ImVec2(0, 0), true, ImGuiWindowFlags_NoBackground);
	{
		switch(m_selectedView)
		{
			case ExplorerViews::ARTISTS:
				ImGui::TextUnformatted("TODO");//TODO
				break;
			case ExplorerViews::ALBUMS:
				ImGui::TextUnformatted("TODO");//TODO
				break;
			case ExplorerViews::MUSICS:
				ImGui::TextUnformatted("TODO");//TODO
				break;
			case ExplorerViews::FILES:
				m_fileExplorer.print();
				break;
		}
	}
	ImGui::EndChild();
}

std::string_view Explorer::ExplorerViewsTxt(Explorer::ExplorerViews explorerViews) const noexcept
{
	switch(explorerViews)
	{
		case ExplorerViews::ARTISTS:
			return "Artists";
		case ExplorerViews::ALBUMS:
			return "Albums";
		case ExplorerViews::MUSICS:
			return "Musics";
		case ExplorerViews::FILES:
			return "Files";
	}
}
