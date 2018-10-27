//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_PATHINFO_HPP
#define MAGICPLAYER_PATHINFO_HPP

#include <filesystem>

// Path wrapper with information needed by GUI already computed
struct PathInfo final
{
	std::filesystem::path path;
	bool is_folder;
	std::string file_name;
	std::uintmax_t file_size;
	bool has_supported_audio_extension;

	PathInfo() noexcept;

	PathInfo(const PathInfo&) = default;
	PathInfo& operator=(const PathInfo&) = default;

	PathInfo(PathInfo&&) noexcept = default;
	PathInfo& operator=(PathInfo&&) noexcept = default;

	~PathInfo() = default;
};

#endif //MAGICPLAYER_PATHINFO_HPP
