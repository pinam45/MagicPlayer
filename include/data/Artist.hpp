//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_ARTIST_HPP
#define MAGICPLAYER_ARTIST_HPP

#include "data/Album.hpp"

#include <string>
#include <vector>
#include <cstdint>

namespace data
{
	struct Artist final
	{
		std::uint64_t id;

		std::string name;

		std::vector<Album> albums;

		explicit Artist(std::string name_) noexcept;

		Artist(const Artist&) = delete;
		Artist& operator=(const Artist&) = delete;

		Artist(Artist&&) noexcept = default;
		Artist& operator=(Artist&&) noexcept = default;

		~Artist() noexcept = default;

		void addAlbum(Album&& album) noexcept;
	};
} // namespace data

#endif //MAGICPLAYER_ARTIST_HPP
