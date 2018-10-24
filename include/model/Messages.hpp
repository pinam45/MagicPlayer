//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_MESSAGES_HPP
#define MAGICPLAYER_MESSAGES_HPP

#include "utils/shared_queue.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <iostream>
#include <iomanip>
#include <string>
#include <variant>
#include <filesystem>

namespace Msg
{

	namespace details
	{
		struct ostream_config_guard
		{
			std::ostream& os;
			std::ios_base::fmtflags flags;

			explicit ostream_config_guard(std::ostream& os);
			~ostream_config_guard();
		};
	} // namespace details

	namespace In
	{

		struct Close
		{
			//TODO: save?
		};
		std::ostream& operator<<(std::ostream& os, const Close& m);

		struct Load
		{
			std::filesystem::path path;

			explicit Load(std::filesystem::path path);
		};
		std::ostream& operator<<(std::ostream& os, const Load& m);

		struct Control
		{
			enum class Action
			{
				PLAY,
				PAUSE,
				STOP,
			};
			Action action;

			explicit Control(Action action);
		};
		std::ostream& operator<<(std::ostream& os, const Control::Action& a);
		std::ostream& operator<<(std::ostream& os, const Control& m);

		struct Volume
		{
			bool muted;
			float volume;

			Volume(bool muted, float volume);
		};
		std::ostream& operator<<(std::ostream& os, const Volume& m);

		struct MusicOffset
		{
			float seconds;

			explicit MusicOffset(float seconds);
		};
		std::ostream& operator<<(std::ostream& os, const MusicOffset& m);

		struct RequestMusicOffset
		{
			//TODO: general Request message with enum?
		};
		std::ostream& operator<<(std::ostream& os, const RequestMusicOffset& m);
	} // namespace In

	namespace Out
	{

		struct MusicOffset
		{
			float seconds;

			explicit MusicOffset(float seconds);
		};
		std::ostream& operator<<(std::ostream& os, const MusicOffset& m);

		struct MusicInfo
		{
			bool valid;
			float durationSeconds;

			MusicInfo(bool valid, float durationSeconds);
		};
		std::ostream& operator<<(std::ostream& os, const MusicInfo& m);
	} // namespace Out

	struct Com
	{
		typedef std::variant<In::Close,
		                     In::Load,
		                     In::Control,
		                     In::Volume,
		                     In::MusicOffset,
		                     In::RequestMusicOffset>
		  InMessage;
		typedef std::variant<Out::MusicOffset, Out::MusicInfo> OutMessage;
		shared_queue<InMessage> in;
		shared_queue<OutMessage, true> out;
	};
} // namespace Msg

inline std::ostream& Msg::In::operator<<(std::ostream& os, [[maybe_unused]] const Msg::In::Close& m)
{
	return os << "Close{}";
}

inline std::ostream& Msg::In::operator<<(std::ostream& os, const Msg::In::Load& m)
{
	return os << "Load{"
	          << "path: " << m.path << "}";
}

inline std::ostream& Msg::In::operator<<(std::ostream& os, const Msg::In::Control::Action& a)
{
	// Not beautiful but only used for logging purpose...
	constexpr const char* ACTION_STR[] = {
	  "PLAY",
	  "PAUSE",
	  "STOP",
	};
	return os << ACTION_STR[static_cast<std::size_t>(a)];
}

inline std::ostream& Msg::In::operator<<(std::ostream& os, const Msg::In::Control& m)
{
	return os << "Control{"
	          << "action: " << m.action << "}";
}

inline std::ostream& Msg::In::operator<<(std::ostream& os, const Msg::In::Volume& m)
{
	Msg::details::ostream_config_guard guard(os);
	return os << "Volume{"
	          << "muted: " << m.muted << ","
	          << "volume:" << m.volume << "}";
}

inline std::ostream& Msg::In::operator<<(std::ostream& os, const Msg::In::MusicOffset& m)
{
	return os << "MusicOffset{"
	          << "seconds: " << m.seconds << "}";
}

inline std::ostream& Msg::In::operator<<(std::ostream& os,
                                         [[maybe_unused]] const Msg::In::RequestMusicOffset& m)
{
	return os << "RequestMusicOffset{}";
}

inline std::ostream& Msg::Out::operator<<(std::ostream& os, const Msg::Out::MusicOffset& m)
{
	return os << "MusicOffset{"
	          << "seconds: " << m.seconds << "}";
}

inline std::ostream& Msg::Out::operator<<(std::ostream& os, const Msg::Out::MusicInfo& m)
{
	Msg::details::ostream_config_guard guard(os);
	return os << "MusicInfo{"
	          << "valid: " << m.valid << ","
	          << "durationSeconds:" << m.durationSeconds << "}";
}

#endif //MAGICPLAYER_MESSAGES_HPP
