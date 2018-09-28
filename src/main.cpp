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

#include <SoundFileReaderMp3.hpp>
#include <imgui_easy_theming.hpp>

#include <imgui.h>
#include <imgui-SFML.h>

#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Audio/SoundFileFactory.hpp>
#include <SFML/Audio/Music.hpp>

#define COUNT_OF(X) (sizeof(X)/sizeof(*(X)))

static const ImVec2 IMGUI_TOP_LEFT_POS = ImVec2(0, 0);

static ImGui::ETheming::ColorTheme setupStyle();

ImGui::ETheming::ColorTheme setupStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 0.0f;
	style.ScrollbarRounding = 0.0f;
	ImGui::ETheming::setColorTheme(ImGui::ETheming::ColorTheme::ArcDark);
	return ImGui::ETheming::ColorTheme::ArcDark;
}

int main()
{
	sf::SoundFileFactory::registerReader<SoundFileReaderMp3>();

	std::array<char, 2018> music_file_path{};
	sf::Music music;
	float volume = 20.f;
	music.setVolume(volume);
	bool valid_music = false;
	bool music_stopped = false;

	sf::Clock deltaClock;

	sf::RenderWindow window(sf::VideoMode(500, 120), "MagicPlayer", sf::Style::Titlebar | sf::Style::Close);
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);
	ImGui::SFML::Init(window);

	ImGui::GetIO().IniFilename = nullptr; // disable .ini saving
	setupStyle();

	ImVec2 window_size = ImVec2{
	  static_cast<float>(window.getSize().x),
	  static_cast<float>(window.getSize().y)
	};

	while(window.isOpen())
	{
		sf::Event event{};
		while(window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			if(event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		ImGui::SFML::Update(window, deltaClock.restart());
		ImGui::SetNextWindowPos(IMGUI_TOP_LEFT_POS);
		window_size.x = static_cast<float>(window.getSize().x);
		window_size.y = static_cast<float>(window.getSize().y);
		ImGui::SetNextWindowSize(window_size);

		ImGui::Begin("Main", nullptr,
		             ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		             ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);

		constexpr char const * const music_path_label_text = "Music file path:";
		ImFont const * const font = ImGui::GetFont();
		ImVec2 text_size = font->CalcTextSizeA(font->FontSize, FLT_MAX, FLT_MAX, music_path_label_text);
		ImGui::PushItemWidth(text_size.x);
		ImGui::LabelText("##file_path_pre", "%s", music_path_label_text);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - text_size.x - 4 * ImGui::GetFontSize());
		bool try_load_music = ImGui::InputText("##file_path", music_file_path.data(), music_file_path.size(), ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		try_load_music |= ImGui::Button("load");

		if(try_load_music){
			music.stop();
			const std::string file_path(music_file_path.data());
			valid_music = music.openFromFile(file_path);
		}

		if(valid_music){
			ImGui::Separator();

			constexpr float trac_min = 0.0f;
			constexpr float trac_max = 100.0f;
			const float playingOffset = music.getPlayingOffset().asSeconds();
			const float duration = music.getDuration().asSeconds();
			float trac_pos = playingOffset / duration * trac_max;
			ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - ImGui::GetFontSize() * 8);
			if(ImGui::SliderScalar("##time", ImGuiDataType_Float, &trac_pos, &trac_min, &trac_max, "%.0f%%")){
				music.setPlayingOffset(sf::seconds(trac_pos / 100 * duration));
			}
			ImGui::PopItemWidth();

			const int playingOffset_i = static_cast<int>(playingOffset);
			const int duration_i = static_cast<int>(duration);
			ImGui::SameLine();
			ImGui::LabelText("##time_post", "%02d:%02d / %02d:%02d", playingOffset_i / 60, playingOffset_i % 60, duration_i / 60, duration_i % 60);

			if(ImGui::Button("play")){
				music.play();
				music_stopped = false;
			}
			ImGui::SameLine();
			if(ImGui::Button("pause")){
				music.pause();
			}
			ImGui::SameLine();
			if(ImGui::Button("stop")){
				music.stop();
				music_stopped = true;
			}
		}

		ImGui::Separator();
		ImGui::PushItemWidth(ImGui::GetFontSize() * -4);
		if(ImGui::SliderFloat("volume", &volume, 0.f, 100.f, "%.1f")){
			music.setVolume(volume);
		}
		ImGui::PopItemWidth();

		ImGui::End();

		window.clear();
		ImGui::SFML::Render(window);
		window.display();

		// If music ended, stop it so play button restart the music
		if(music.getStatus() == sf::Music::Stopped && !music_stopped){
			music_stopped = true;
			music.stop();
		}
	}

	music.stop();

	ImGui::SFML::Shutdown();
	return EXIT_SUCCESS;
}
