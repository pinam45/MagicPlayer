//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "data/Music.hpp"

data::Music::Music(int track_,
                   std::string title_,
                   std::chrono::duration<int> length_,
                   std::filesystem::path path_) noexcept
  : id()
  , album(nullptr)
  , track(track_)
  , title(std::move(title_))
  , length(length_)
  , path(std::move(path_))
{
}
