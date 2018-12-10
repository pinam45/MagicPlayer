//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "view/imgui_custom_widgets.hpp"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#	define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui_internal.h>

bool ImGuiCW::PlayerBar(const char* label,
                        float* v,
                        const float min,
                        const float max,
                        const ImVec2& size_arg)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if(window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	ImVec2 pos = window->DC.CursorPos;
	ImRect bb(pos,
	          pos
	            + ImGui::CalcItemSize(
	              size_arg, ImGui::CalcItemWidth(), g.FontSize + style.FramePadding.y * 2.0f));
	ImGui::ItemSize(bb, style.FramePadding.y);
	if(!ImGui::ItemAdd(bb, id))
		return false;

	const bool tab_focus_requested = ImGui::FocusableItemRegister(window, id);
	const bool hovered = ImGui::ItemHoverable(bb, id);
	if(tab_focus_requested || (hovered && g.IO.MouseClicked[0]) || g.NavActivateId == id
	   || g.NavInputId == id)
	{
		ImGui::SetActiveID(id, window);
		ImGui::SetFocusID(id, window);
		ImGui::FocusWindow(window);
	}

	// Behavior
	bool value_changed = false;
	if(g.ActiveId == id)
	{
		if(g.ActiveIdSource == ImGuiInputSource_Mouse)
		{
			if(!g.IO.MouseDown[0])
			{
				ImGui::ClearActiveID();
			}
			else
			{
				const float clicked_t =
				  ImClamp((g.IO.MousePos.x - bb.Min.x) / bb.GetWidth(), 0.0f, 1.0f);
				*v = ImLerp(min, max, clicked_t);
				value_changed = true;
			}
		}
		else if(g.ActiveIdSource == ImGuiInputSource_Nav)
		{
			//TODO: not yet supported
		}
	}

	if(value_changed)
	{
		ImGui::MarkItemEdited(id);
	}

	// Render
	float fraction = ImSaturate((*v - min) / (max - min));
	ImGui::RenderFrame(
	  bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);
	bb.Expand(ImVec2(-style.FrameBorderSize, -style.FrameBorderSize));
	ImGui::RenderRectFilledRangeH(window->DrawList,
	                              bb,
	                              ImGui::GetColorU32(ImGuiCol_PlotHistogram),
	                              0.0f,
	                              fraction,
	                              style.FrameRounding);

	return value_changed;
}
