//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "data/Artist.hpp"

#include <cassert>

std::vector<const data::Music*> data::Artist::findMusics(id::type search_id) const noexcept
{
	assert(search_id != id::INVALID);
	std::vector<const data::Music*> found_musics;

	if(search_id == id)
	{
		// return all musics of the artist
		for(const Album& album: albums)
		{
			for(const Music& music: album.musics)
			{
				found_musics.push_back(&music);
			}
		}
		return found_musics;
	}

	// search album
	std::vector<Album>::const_iterator it = std::lower_bound(
	  albums.cbegin(), albums.cend(), search_id, [](const Album& album, id::type wanted_id) {
		  return album.id < wanted_id;
	  });
	if(it == albums.cend())
	{
		return found_musics;
	}

	if(it->id == search_id)
	{
		// return all musics of the album
		for(const Music& music: it->musics)
		{
			found_musics.push_back(&music);
		}
		return found_musics;
	}

	const Music* music = it->findMusic(search_id);
	if(music != nullptr)
	{
		// return a music of the album
		found_musics.push_back(music);
	}

	return found_musics;
}

data::Artist::Artist(std::string name_) noexcept: id(), name(std::move(name_)), albums()
{
}

void data::Artist::addAlbum(Album&& album) noexcept
{
	assert(album.artist == nullptr || album.artist == this);
	album.artist = this;
	albums.push_back(std::move(album));
}
