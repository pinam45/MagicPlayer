//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_IMGUI_CUSTOM_WIDGETS_HPP
#define MAGICPLAYER_IMGUI_CUSTOM_WIDGETS_HPP

#include <imgui.h>

namespace ImGuiCW
{
	bool PlayerBar(const char* label,
	               float* v,
	               float min,
	               float max,
	               const ImVec2& size_arg = ImVec2(-1, 0));
}

#endif //MAGICPLAYER_IMGUI_CUSTOM_WIDGETS_HPP
