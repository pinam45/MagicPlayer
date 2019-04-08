//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_MUSIC_HPP
#define MAGICPLAYER_MUSIC_HPP

#include "utils/path_utils.hpp"
#include "utils/IdGenerator.hpp"

#include <string>
#include <chrono>
#include <cstdint>

namespace data
{
	struct Album;

	struct Music final
	{
		id::type id;
		const Album* album;

		int track;
		std::string title;
		std::chrono::duration<int> length;

		utf8_path path;

		Music(const Music&) = delete;
		Music& operator=(const Music&) = delete;

		Music(Music&&) noexcept = default;
		Music& operator=(Music&& m) noexcept = default;

		~Music() noexcept = default;

	private:
		friend class DataManager;

		Music(int track,
		      std::string title,
		      std::chrono::duration<int> length,
		      utf8_path path) noexcept;
	};
} // namespace data

#endif //MAGICPLAYER_MUSIC_HPP
