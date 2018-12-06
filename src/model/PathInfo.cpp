//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "model/PathInfo.hpp"
#include "utils/ostream_config_guard.hpp"

#include <iomanip>

PathInfo::PathInfo() noexcept
  : path(), is_folder(false), file_name(), file_size(0), has_supported_audio_extension(false)
{
}

std::ostream& operator<<(std::ostream& os, const PathInfo& p)
{
	ostream_config_guard guard(os, std::boolalpha, std::fixed, std::setprecision(2));
	return os << "PathInfo{"
	          << "path: " << p.path << ","
	          << "is_folder: " << p.is_folder << ","
	          << "file_name: " << p.file_name << ","
	          << "file_size: " << p.file_size << ","
	          << "has_supported_audio_extension: " << p.has_supported_audio_extension << "}";
}
