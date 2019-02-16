//
// Copyright (c) 2019 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_EXPLORER_HPP
#define MAGICPLAYER_EXPLORER_HPP

#include "model/Messages.hpp"
#include "view/windows/explorers/FileExplorer.hpp"

#include <spdlog/logger.h>

class Explorer final
{
public:
	Explorer(std::string name, Msg::Sender sender);

	void init();

	void show();

	void processMessage(Msg::Out::FolderContent& message);

	void processMessage(Msg::Out::Database& message);

private:
	void showLeftPanel() noexcept;

	void showRightPanel() noexcept;

	enum class ExplorerViews
	{
		ARTISTS,
		ALBUMS,
		MUSICS,
		FILES
	};
	std::string_view ExplorerViewsTxt(ExplorerViews explorerViews) const noexcept;

	Msg::Sender m_sender;

	std::string m_name;
	ExplorerViews m_selectedView;
	FileExplorer m_fileExplorer;

	std::shared_ptr<spdlog::logger> m_logger;
};

#endif //MAGICPLAYER_EXPLORER_HPP
