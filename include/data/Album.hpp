//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_ALBUM_HPP
#define MAGICPLAYER_ALBUM_HPP

#include "data/Music.hpp"
#include "utils/IdGenerator.hpp"

#include <SFML/Graphics/Image.hpp>

#include <string>
#include <chrono>
#include <vector>
#include <memory>
#include <cstdint>

namespace data
{
	struct Artist;

	struct Album final
	{
		id::type id;
		const Artist* artist;

		std::string name;
		std::string genre;
		int year;
		std::chrono::duration<int> length;
		// std::unique_ptr because sf::Image is not movable
		std::unique_ptr<sf::Image> image;
		std::vector<Music> musics;

		Album(const Album&) = delete;
		Album& operator=(const Album&) = delete;

		Album(Album&&) noexcept = default;
		Album& operator=(Album&&) noexcept = default;

		~Album() noexcept = default;

		const Music* findMusic(id::type id) const noexcept;

	private:
		friend class DataManager;

		Album(std::string name, std::string genre, int year) noexcept;

		Album(std::string name,
		      std::string genre,
		      int year,
		      const void* image_data,
		      std::size_t image_size) noexcept;

		void addMusic(Music&& music) noexcept;
	};
} // namespace data

#endif //MAGICPLAYER_ALBUM_HPP
