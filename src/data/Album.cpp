//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "data/Album.hpp"
#include "utils/IdGenerator.hpp"

#include <utility>
#include <cassert>

const data::Music* data::Album::findMusic(std::uint64_t search_id) const noexcept
{
	assert(search_id != IdGenerator::INVALID_ID);
	if(id <= search_id)
	{
		return nullptr;
	}
	size_t pos = search_id - id;
	if(pos >= musics.size())
	{
		return nullptr;
	}
	return &musics[pos];
}

data::Album::Album(std::string name_, std::string genre_, int year_) noexcept
  : id()
  , artist(nullptr)
  , name(std::move(name_))
  , genre(std::move(genre_))
  , year(year_)
  , length()
  , image(std::make_unique<sf::Image>())
  , musics()
{
	image->create(1, 1);
}

data::Album::Album(std::string name_,
                   std::string genre_,
                   int year_,
                   const void* image_data,
                   std::size_t image_size) noexcept
  : id()
  , artist(nullptr)
  , name(std::move(name_))
  , genre(std::move(genre_))
  , year(year_)
  , length()
  , image(std::make_unique<sf::Image>())
  , musics()
{
	image->loadFromMemory(image_data, image_size);
}

void data::Album::addMusic(data::Music&& music) noexcept
{
	assert(music.album == nullptr || music.album == this);
	music.album = this;
	length += music.length;
	musics.push_back(std::move(music));
}
