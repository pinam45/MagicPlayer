//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//

// Based on functions from gpulib by procedural (https://github.com/procedural/gpulib)

#ifndef IMGUI_EASY_THEMING_HPP
#define IMGUI_EASY_THEMING_HPP


namespace ImGui::ETheming{

	struct Color
	{
		float r;
		float g;
		float b;

		constexpr Color(int r, int g, int b) noexcept;

		constexpr Color(const Color&) noexcept = default;

		Color& operator=(const Color&) noexcept = default;
	};

	struct ColorTheme
	{
		Color text;
		Color head;
		Color area;
		Color body;
		Color pops;

		constexpr ColorTheme(Color text, Color head, Color area, Color body, Color pops) noexcept;

		constexpr ColorTheme(const ColorTheme&) noexcept = default;

		ColorTheme& operator=(const ColorTheme&) noexcept = default;

		// Arc Dark: https://github.com/horst3180/arc-theme
		static const ColorTheme ArcDark;

		// Flat UI by yorick.penninks: https://color.adobe.com/Flat-UI-color-theme-2469224/
		static const ColorTheme FlatUI;

		// Mint-Y-Dark: https://github.com/linuxmint/mint-y-theme
		static const ColorTheme MintYDark;

		// Custom light theme
		static const ColorTheme LightBlue;
	};

	void setColorTheme(const ColorTheme& colorTheme);

	bool showThemeColorsChooser(ColorTheme* p_colorTheme);

	void showThemeColorsManager(ColorTheme* p_colorTheme);

	void showPredefinedThemeSelector(ColorTheme* p_colorTheme);

	void showThemeConfigWindow(ColorTheme* p_colorTheme, bool* p_open);
}


namespace ImGui::ETheming{

	constexpr Color::Color(
	  int r_,
	  int g_,
	  int b_
	) noexcept
	  : r(static_cast<float>(r_) / 255.f)
	  , g(static_cast<float>(g_) / 255.f)
	  , b(static_cast<float>(b_) / 255.f)
	{
	}

	constexpr ColorTheme::ColorTheme(
	  Color text_,
	  Color head_,
	  Color area_,
	  Color body_,
	  Color pops_
	) noexcept
	  : text(text_)
	  , head(head_)
	  , area(area_)
	  , body(body_)
	  , pops(pops_)
	{
	}
}


#endif //IMGUI_EASY_THEMING_HPP
