//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "model/PathInfo.hpp"

PathInfo::PathInfo() noexcept
  : path(), is_folder(false), file_name(), file_size(0), has_supported_audio_extension(false)
{
}
