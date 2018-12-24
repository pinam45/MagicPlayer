//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_MUSIC_HPP
#define MAGICPLAYER_MUSIC_HPP

#include <string>
#include <chrono>
#include <filesystem>
#include <cstdint>

namespace data
{
	struct Album;

	struct Music final
	{
		std::uint64_t id;
		const Album* album;

		int track;
		std::string title;
		std::chrono::duration<int> length;

		std::filesystem::path path;

		Music(int track_,
		      std::string title_,
		      std::chrono::duration<int> length_,
		      std::filesystem::path path_) noexcept;

		Music(const Music&) = delete;
		Music& operator=(const Music&) = delete;

		Music(Music&&) noexcept = default;
		Music& operator=(Music&& m) noexcept = default;

		~Music() noexcept = default;
	};
} // namespace data

#endif //MAGICPLAYER_MUSIC_HPP
