//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "model/Messages.hpp"

Msg::In::Open::Open(std::filesystem::path path_): path(std::move(path_))
{
}

Msg::In::Control::Control(Control::Action action_): action(action_)
{
}

Msg::In::Volume::Volume(bool muted_, float volume_): muted(muted_), volume(volume_)
{
}

Msg::In::MusicOffset::MusicOffset(float seconds_): seconds(seconds_)
{
}

Msg::Out::MusicOffset::MusicOffset(float seconds_): seconds(seconds_)
{
}

Msg::Out::MusicInfo::MusicInfo(bool valid_, float durationSeconds_)
  : valid(valid_), durationSeconds(durationSeconds_)
{
}

Msg::Out::FolderContent::FolderContent(std::filesystem::path path_,
                                       const std::vector<PathInfo>& content_)
  : path(std::move(path_)), content(content_)
{
}

Msg::Out::FolderContent::FolderContent(std::filesystem::path path_,
                                       std::vector<PathInfo>&& content_)
  : path(std::move(path_)), content(std::move(content_))
{
}

Msg::Sender::Sender(Msg::Com& com) noexcept: m_com(com)
{
}
