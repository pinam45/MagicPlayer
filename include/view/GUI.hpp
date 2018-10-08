/*****************************************************************************************
 *                                                                                       *
 * MIT License                                                                           *
 *                                                                                       *
 * Copyright (c) 2018 Maxime Pinard                                                      *
 *                                                                                       *
 * Permission is hereby granted, free of charge, to any person obtaining a copy          *
 * of this software and associated documentation files (the "Software"), to deal         *
 * in the Software without restriction, including without limitation the rights          *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell             *
 * copies of the Software, and to permit persons to whom the Software is                 *
 * furnished to do so, subject to the following conditions:                              *
 *                                                                                       *
 * The above copyright notice and this permission notice shall be included in all        *
 * copies or substantial portions of the Software.                                       *
 *                                                                                       *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR            *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,              *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE           *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER                *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,         *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE         *
 * SOFTWARE.                                                                             *
 *                                                                                       *
 *****************************************************************************************/

#ifndef MAGICPLAYER_GUI_HPP
#define MAGICPLAYER_GUI_HPP


#include "model/Messages.hpp"
#include "imgui_easy_theming.hpp"

#include <spdlog/logger.h>

#include <array>

class GUI {

public:

	explicit GUI(Msg::Com& com);

	template<typename Message>
	void handleMessage(Message& message) = delete;

	int run();

private:

	void show();

	struct MusicInfos{
		bool valid;
		float offset;
		float duration;
	};
	MusicInfos m_musicInfos;

	Msg::Com& m_com;
	bool m_showThemeConfigWindow;
	std::array<char, 2018> m_music_file_path;
	float m_volume;
	ImGui::ETheming::ColorTheme m_style;

	std::shared_ptr<spdlog::logger> m_logger;
};


#endif //MAGICPLAYER_GUI_HPP
