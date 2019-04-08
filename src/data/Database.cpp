//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "data/Database.hpp"

#include <cassert>

data::Database::Database() noexcept: id(), sources(), artists()
{
}

std::vector<const data::Music*> data::Database::findMusics(id::type search_id) const noexcept
{
	assert(search_id != id::INVALID);
	std::vector<const data::Music*> found_musics;

	if(search_id == id)
	{
		// return all musics of the database
		for(const Artist& artist: artists)
		{
			for(const Album& album: artist.albums)
			{
				for(const Music& music: album.musics)
				{
					found_musics.push_back(&music);
				}
			}
		}
		return found_musics;
	}

	// search artist
	std::vector<Artist>::const_iterator it = std::lower_bound(
	  artists.cbegin(), artists.cend(), search_id, [](const Artist& artist, id::type wanted_id) {
		  return artist.id < wanted_id;
	  });
	if(it == artists.cend())
	{
		return found_musics;
	}

	return it->findMusics(search_id);
}
