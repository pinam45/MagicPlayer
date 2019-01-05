//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "utils/log.hpp"
#include "model/Logic.hpp"
#include "view/GUI.hpp"
#include "SoundFileReaderMp3.hpp"

#include <iostream>
#include <thread>

int main()
{
	std::ios_base::sync_with_stdio(false);

	if(!init_logger())
	{
		return EXIT_FAILURE;
	}

	spdlog::get(GENERAL_LOGGER_NAME)->info("MagicPlayer started");
	{
		Logic logic;
		std::thread logicThread([&]() { logic.run(); });

		GUI gui(logic.getCom());
		gui.run();

		logicThread.join();
	}
	spdlog::get(GENERAL_LOGGER_NAME)->info("MagicPlayer ended");

	return EXIT_SUCCESS;
}
