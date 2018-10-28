//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "view/imgui_helpers.hpp"

#include <imgui.h>

#include <limits>

float compute_text_width(char const* const text)
{
	ImFont const* const font = ImGui::GetFont();
	return font
	  ->CalcTextSizeA(
	    font->FontSize, std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), text)
	  .x;
}
