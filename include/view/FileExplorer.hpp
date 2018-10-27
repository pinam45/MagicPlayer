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

#include <spdlog/logger.h>

#include <filesystem>

class FileExplorer final
{
public:
	FileExplorer(std::string name, shared_queue<Msg::Com::InMessage>& com_in);

	void init();

	void show();

	void processMessage(Msg::Out::FolderContent& message);

private:
	template<typename Message, typename... Args>
	void sendMessage(Args&&... args);

	shared_queue<Msg::Com::InMessage>& m_com_in;

	std::string m_name;
	std::array<char, 2018> m_music_file_path;
	std::filesystem::path m_path;
	std::vector<PathInfo> m_content;

	std::shared_ptr<spdlog::logger> m_logger;
};

#endif //MAGICPLAYER_FILEEXPLORER_HPP
