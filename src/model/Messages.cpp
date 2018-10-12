/*****************************************************************************************
 *                                                                                       *
 * MIT License                                                                           *
 *                                                                                       *
 * Copyright (c) 2018 Maxime Pinard                                                      *
 *                                                                                       *
 * Permission is hereby granted, free of charge, to any person obtaining a copy          *
 * of this software and associated documentation files (the "Software"), to deal         *
 * in the Software without restriction, including without limitation the rights          *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell             *
 * copies of the Software, and to permit persons to whom the Software is                 *
 * furnished to do so, subject to the following conditions:                              *
 *                                                                                       *
 * The above copyright notice and this permission notice shall be included in all        *
 * copies or substantial portions of the Software.                                       *
 *                                                                                       *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR            *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,              *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE           *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER                *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,         *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE         *
 * SOFTWARE.                                                                             *
 *                                                                                       *
 *****************************************************************************************/

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
