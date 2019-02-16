//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_FILEEXPLORER_HPP
#define MAGICPLAYER_FILEEXPLORER_HPP

#include "model/Messages.hpp"
#include "model/PathInfo.hpp"
#include "utils/path_utils.hpp"

#include <spdlog/logger.h>

class FileExplorer final
{
public:
	explicit FileExplorer(Msg::Sender sender);

	void init();

	void print();

	void processMessage(Msg::Out::FolderContent& message);

private:
	enum class focus_state
	{
		NOT_FOCUSED,
		FOCUS_REQUESTED,
		FOCUSED
	};

	Msg::Sender m_sender;

	std::array<char, 2018> m_user_path;
	utf8_path m_path;
	std::vector<PathInfo> m_content;
	std::vector<std::string> m_formatted_content;
	std::size_t selected_content;
	std::vector<std::tuple<std::string, utf8_path>> m_sub_paths;
	std::size_t m_displayable_split_paths;
	float m_last_available_width;
	focus_state m_text_field_focus_state;

	std::shared_ptr<spdlog::logger> m_logger;
};

#endif //MAGICPLAYER_FILEEXPLORER_HPP
