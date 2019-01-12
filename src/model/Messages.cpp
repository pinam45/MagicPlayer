//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "model/Messages.hpp"
#include "utils/log.hpp"

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

Msg::In::Settings::Settings(data::Settings settings_, bool save_)
  : settings(std::move(settings_)), save(save_)
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

Msg::Out::Database::Database(std::shared_ptr<const data::Database> database_)
  : database(std::move(database_))
{
}

Msg::Out::Settings::Settings(data::Settings settings_): settings(std::move(settings_))
{
}

Msg::Sender::Sender(Msg::Com& com) noexcept: m_com(com)
{
}

std::ostream& Msg::In::operator<<(std::ostream& os, [[maybe_unused]] const Msg::In::Close& m)
{
	return os << "Close{}";
}

std::ostream& Msg::In::operator<<(std::ostream& os, const Msg::In::Open& m)
{
	return os << "Open{"
	          << "path: " << m.path << "}";
}

std::ostream& Msg::In::operator<<(std::ostream& os, const Msg::In::Control::Action& a)
{
	// Not beautiful but only used for logging purpose...
	constexpr const char* ACTION_STR[] = {
	  "PLAY",
	  "PAUSE",
	  "STOP",
	};
	return os << ACTION_STR[static_cast<std::size_t>(a)];
}

std::ostream& Msg::In::operator<<(std::ostream& os, const Msg::In::Control& m)
{
	return os << "Control{"
	          << "action: " << m.action << "}";
}

std::ostream& Msg::In::operator<<(std::ostream& os, const Msg::In::Volume& m)
{
	ostream_config_guard guard(os, std::boolalpha, std::fixed, std::setprecision(2));
	return os << "Volume{"
	          << "muted: " << m.muted << ","
	          << "volume:" << m.volume << "}";
}

std::ostream& Msg::In::operator<<(std::ostream& os, const Msg::In::MusicOffset& m)
{
	return os << "MusicOffset{"
	          << "seconds: " << m.seconds << "}";
}

std::ostream& Msg::In::operator<<(std::ostream& os,
                                  [[maybe_unused]] const Msg::In::RequestMusicOffset& m)
{
	return os << "RequestMusicOffset{}";
}

std::ostream& Msg::In::operator<<(std::ostream& os, [[maybe_unused]] const Msg::In::Settings& m)
{
	ostream_config_guard guard(os, std::boolalpha);
	return os << "Settings{"
	          << "settings: " << m.settings << ","
	          << "save: " << m.save << "}";
}

std::ostream& Msg::In::operator<<(std::ostream& os,
                                  [[maybe_unused]] const Msg::In::InnerTaskEnded& m)
{
	return os << "InnerTaskEnded{}";
}

std::ostream& Msg::Out::operator<<(std::ostream& os, const Msg::Out::MusicOffset& m)
{
	return os << "MusicOffset{"
	          << "seconds: " << m.seconds << "}";
}

std::ostream& Msg::Out::operator<<(std::ostream& os, const Msg::Out::MusicInfo& m)
{
	ostream_config_guard guard(os, std::boolalpha, std::fixed, std::setprecision(2));
	return os << "MusicInfo{"
	          << "valid: " << m.valid << ","
	          << "durationSeconds:" << m.durationSeconds << "}";
}

std::ostream& Msg::Out::operator<<(std::ostream& os, const Msg::Out::FolderContent& m)
{
	os << "FolderContent{"
	   << "path: " << m.path << ","
	   << "content: [";
	for(const auto& info: m.content)
	{
		os << info << ",";
	}
	os << "]}";

	return os;
}

std::ostream& Msg::Out::operator<<(std::ostream& os, const Msg::Out::Database& m)
{
	os << "Database{"
	   << "id: " << m.database->id << ","
	   << "sources: [";
	for(const auto& source: m.database->sources)
	{
		os << source << ",";
	}
	os << "]}";

	return os;
}

std::ostream& Msg::Out::operator<<(std::ostream& os, [[maybe_unused]] const Msg::Out::Settings& m)
{
	return os << "Settings{"
	          << "settings: " << m.settings << "}";
}
