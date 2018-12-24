//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "data/Artist.hpp"

#include <cassert>

data::Artist::Artist(std::string name_) noexcept: id(), name(std::move(name_)), albums()
{
}

void data::Artist::addAlbum(Album&& album) noexcept
{
	assert(album.artist == nullptr || album.artist == this);
	album.artist = this;
	albums.push_back(std::move(album));
}
