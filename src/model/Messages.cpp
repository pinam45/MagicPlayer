//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "model/Messages.hpp"

Msg::details::ostream_config_guard::ostream_config_guard(std::ostream& os_)
  : os(os_)
    , flags(os_.flags()) {
	os << std::boolalpha
	   << std::fixed
	   << std::setprecision(2);
}

Msg::details::ostream_config_guard::~ostream_config_guard() {
	os.flags(flags);
}

Msg::In::Load::Load(std::filesystem::path path_)
  : path(std::move(path_)) {

}

Msg::In::Control::Control(Control::Action action_)
  : action(action_) {

}

Msg::In::Volume::Volume(bool muted_, float volume_)
  : muted(muted_)
  , volume(volume_) {

}

Msg::In::MusicOffset::MusicOffset(float seconds_)
  : seconds(seconds_) {

}

Msg::Out::MusicOffset::MusicOffset(float seconds_)
  : seconds(seconds_) {

}

Msg::Out::MusicInfo::MusicInfo(bool valid_, float durationSeconds_)
  : valid(valid_)
  , durationSeconds(durationSeconds_) {

}
