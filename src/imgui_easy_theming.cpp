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

// Based on functions from gpulib by procedural (https://github.com/procedural/gpulib)

#include <imgui_easy_theming.hpp>

#include <imgui.h>

#define COUNT_OF(X) (sizeof(X)/sizeof(*(X)))

namespace ImGui::ETheming{

	// Arc Dark: https://github.com/horst3180/arc-theme
	const ColorTheme ColorTheme::ArcDark{
	  Color{211, 218, 227}, // text
	  Color{64, 132, 214}, // head
	  Color{47, 52, 63}, // area
	  Color{56, 60, 74}, // body
	  Color{28, 30, 37} // pops
	};

	// Flat UI by yorick.penninks: https://color.adobe.com/Flat-UI-color-theme-2469224/
	const ColorTheme ColorTheme::FlatUI{
	  Color{236, 240, 241}, // text
	  Color{41, 128, 185}, // head
	  Color{57, 79, 105}, // area
	  Color{44, 62, 80}, // body
	  Color{33, 46, 60} // pops
	};

	// Mint-Y-Dark: https://github.com/linuxmint/mint-y-theme
	const ColorTheme ColorTheme::MintYDark{
	  Color{211, 211, 211}, // text
	  Color{95, 142, 85}, // head
	  Color{47, 47, 47}, // area
	  Color{64, 64, 64}, // body
	  Color{30, 30, 30} // pops
	};

	void setColorTheme(const ColorTheme& theme)
	{
		ImGuiStyle& style = ImGui::GetStyle();

		style.Colors[ImGuiCol_Text] = ImVec4(theme.text.r, theme.text.g, theme.text.b, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(theme.text.r, theme.text.g, theme.text.b, 0.58f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(theme.body.r, theme.body.g, theme.body.b, 1.00f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(theme.area.r, theme.area.g, theme.area.b, 0.58f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(theme.pops.r, theme.pops.g, theme.pops.b, 0.92f);
		style.Colors[ImGuiCol_Border] = ImVec4(theme.body.r, theme.body.g, theme.body.b, 0.00f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(theme.body.r, theme.body.g, theme.body.b, 0.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(theme.area.r, theme.area.g, theme.area.b, 1.00f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(theme.head.r, theme.head.g, theme.head.b, 0.20f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(theme.head.r, theme.head.g, theme.head.b, 0.35f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(theme.area.r, theme.area.g, theme.area.b, 1.00f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(theme.area.r, theme.area.g, theme.area.b, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(theme.area.r, theme.area.g, theme.area.b, 0.75f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(theme.area.r, theme.area.g, theme.area.b, 0.47f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(theme.area.r, theme.area.g, theme.area.b, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(theme.head.r, theme.head.g, theme.head.b, 0.21f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(theme.head.r, theme.head.g, theme.head.b, 0.78f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(theme.head.r, theme.head.g, theme.head.b, 1.00f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(theme.head.r, theme.head.g, theme.head.b, 0.80f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(theme.head.r, theme.head.g, theme.head.b, 0.50f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(theme.head.r, theme.head.g, theme.head.b, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(theme.head.r, theme.head.g, theme.head.b, 0.50f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(theme.head.r, theme.head.g, theme.head.b, 0.86f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(theme.head.r, theme.head.g, theme.head.b, 1.00f);
		style.Colors[ImGuiCol_Header] = ImVec4(theme.head.r, theme.head.g, theme.head.b, 0.76f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(theme.head.r, theme.head.g, theme.head.b, 0.86f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(theme.head.r, theme.head.g, theme.head.b, 1.00f);
		style.Colors[ImGuiCol_Separator] = ImVec4(theme.head.r, theme.head.g, theme.head.b, 0.32f);
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(theme.head.r, theme.head.g, theme.head.b, 0.78f);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(theme.head.r, theme.head.g, theme.head.b, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(theme.head.r, theme.head.g, theme.head.b, 0.15f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(theme.head.r, theme.head.g, theme.head.b, 0.78f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(theme.head.r, theme.head.g, theme.head.b, 1.00f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(theme.text.r, theme.text.g, theme.text.b, 0.63f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(theme.head.r, theme.head.g, theme.head.b, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(theme.text.r, theme.text.g, theme.text.b, 0.63f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(theme.head.r, theme.head.g, theme.head.b, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(theme.head.r, theme.head.g, theme.head.b, 0.43f);
		style.Colors[ImGuiCol_DragDropTarget] = ImVec4(theme.text.r, theme.text.g, theme.text.b, 1.00f);
		style.Colors[ImGuiCol_NavHighlight] = ImVec4(theme.text.r, theme.text.g, theme.text.b, 1.00f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(theme.text.r, theme.text.g, theme.text.b, 1.00f);
		style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(theme.text.r, theme.text.g, theme.text.b, 0.20f);
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(theme.text.r, theme.text.g, theme.text.b, 0.35f);
	}

	bool showThemeColorsChooser(ColorTheme* p_colorTheme){
		bool modified = false;

		ImGui::PushItemWidth(ImGui::GetFontSize() * -3);
		modified |= ImGui::ColorEdit3("Text", reinterpret_cast<float*>(&p_colorTheme->text), ImGuiColorEditFlags_NoAlpha);
		modified |= ImGui::ColorEdit3("Head", reinterpret_cast<float*>(&p_colorTheme->head), ImGuiColorEditFlags_NoAlpha);
		modified |= ImGui::ColorEdit3("Area", reinterpret_cast<float*>(&p_colorTheme->area), ImGuiColorEditFlags_NoAlpha);
		modified |= ImGui::ColorEdit3("Body", reinterpret_cast<float*>(&p_colorTheme->body), ImGuiColorEditFlags_NoAlpha);
		modified |= ImGui::ColorEdit3("Pops", reinterpret_cast<float*>(&p_colorTheme->pops), ImGuiColorEditFlags_NoAlpha);
		ImGui::PopItemWidth();

		return modified;
	}

	void showThemeColorsManager(ColorTheme* p_colorTheme){
		static bool auto_apply_style = false;
		static bool apply_style = false;

		apply_style = false;
		bool modified = showThemeColorsChooser(p_colorTheme);
		if(!auto_apply_style){
			apply_style = ImGui::Button("Apply");
			ImGui::SameLine();
		}
		bool old_auto_apply_style = auto_apply_style;
		ImGui::Checkbox("Auto apply style", &auto_apply_style);
		apply_style |= !old_auto_apply_style && (old_auto_apply_style ^ auto_apply_style);

		if(apply_style || (auto_apply_style && modified)){
			setColorTheme(*p_colorTheme);
		}
	}

	void showPredefinedThemeSelector(ColorTheme* p_colorTheme){
		static const char* predefined_styles_names[] = {
		  "Arc Dark",
		  "Flat UI",
		  "Mint-Y-Dark",
		};
		static const ColorTheme predefined_styles[] = {
		  ColorTheme::ArcDark,
		  ColorTheme::FlatUI,
		  ColorTheme::MintYDark,
		};
		static int predefined_style_number = COUNT_OF(predefined_styles);

		ImGui::PushItemWidth(ImGui::GetFontSize() * -4);
		ImGui::Combo("##predefined_styles", &predefined_style_number, predefined_styles_names, COUNT_OF(predefined_styles_names));
		ImGui::SameLine();
		if(ImGui::Button("Apply") && predefined_style_number < static_cast<int>(COUNT_OF(predefined_styles))){
			setColorTheme(predefined_styles[predefined_style_number]);
			*p_colorTheme = predefined_styles[predefined_style_number];
		}
		ImGui::PopItemWidth();
	}

	void showThemeConfigWindow(ColorTheme* p_colorTheme, bool* p_open){
		ImGui::SetNextWindowSize(ImVec2(320, 0), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Theme config", p_open))
		{
			ImGui::End();
			ImGui::PopID();
			return;
		}

		ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
		if(ImGui::TreeNode("Predefined styles")){
			showPredefinedThemeSelector(p_colorTheme);
			ImGui::TreePop();
		}
		ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
		if(ImGui::TreeNode("Custom Style")){
			showThemeColorsManager(p_colorTheme);
			ImGui::TreePop();
		}

		ImGui::End();
	}
}
