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
#include "utils/ostream_config_guard.hpp"
#include "model/PathInfo.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <iostream>
#include <iomanip>
#include <string>
#include <variant>
#include <filesystem>

//TODO: add id to messages to make request/answer connection

namespace Msg
{
	namespace In
	{

		struct Close
		{
			//TODO: save?
		};
		std::ostream& operator<<(std::ostream& os, const Close& m);

		struct Open
		{
			std::filesystem::path path;

			explicit Open(std::filesystem::path path);
		};
		std::ostream& operator<<(std::ostream& os, const Open& m);

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
		};
		std::ostream& operator<<(std::ostream& os, const RequestMusicOffset& m);

		struct InnerTaskEnded
		{
		};
		std::ostream& operator<<(std::ostream& os, const InnerTaskEnded& m);
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

		struct FolderContent
		{
			std::filesystem::path path;
			std::vector<PathInfo> content;

			explicit FolderContent(std::filesystem::path path,
			                       const std::vector<PathInfo>& content = {});
			FolderContent(std::filesystem::path path, std::vector<PathInfo>&& content);
		};
		std::ostream& operator<<(std::ostream& os, const FolderContent& m);
	} // namespace Out

	struct Com final
	{
		typedef std::variant<In::Close,
		                     In::Open,
		                     In::Control,
		                     In::Volume,
		                     In::MusicOffset,
		                     In::RequestMusicOffset,
		                     In::InnerTaskEnded>
		  InMessage;
		typedef std::variant<Out::MusicOffset, Out::MusicInfo, Out::FolderContent> OutMessage;
		shared_queue<InMessage> in;
		shared_queue<OutMessage, true> out;

		template<typename Message, typename... Args>
		void sendInMessage(Args&&... args);

		template<typename Message, typename... Args>
		void sendOutMessage(Args&&... args);
	};
} // namespace Msg

template<typename Message, typename... Args>
void Msg::Com::sendInMessage(Args&& ... args) {
	in.emplace_back(std::in_place_type_t<Message>{}, std::forward<Args>(args)...);
}

template<typename Message, typename... Args>
void Msg::Com::sendOutMessage(Args&& ... args) {
	out.emplace_back(std::in_place_type_t<Message>{}, std::forward<Args>(args)...);
}
inline std::ostream& Msg::In::operator<<(std::ostream& os, [[maybe_unused]] const Msg::In::Close& m)
{
	return os << "Close{}";
}

inline std::ostream& Msg::In::operator<<(std::ostream& os, const Msg::In::Open& m)
{
	return os << "Open{"
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
	ostream_config_guard guard(os, std::boolalpha, std::fixed, std::setprecision(2));
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

inline std::ostream& Msg::In::operator<<(std::ostream& os,
                                         [[maybe_unused]] const Msg::In::InnerTaskEnded& m)
{
	return os << "InnerTaskEnded{}";
}

inline std::ostream& Msg::Out::operator<<(std::ostream& os, const Msg::Out::MusicOffset& m)
{
	return os << "MusicOffset{"
	          << "seconds: " << m.seconds << "}";
}

inline std::ostream& Msg::Out::operator<<(std::ostream& os, const Msg::Out::MusicInfo& m)
{
	ostream_config_guard guard(os, std::boolalpha, std::fixed, std::setprecision(2));
	return os << "MusicInfo{"
	          << "valid: " << m.valid << ","
	          << "durationSeconds:" << m.durationSeconds << "}";
}

inline std::ostream& Msg::Out::operator<<(std::ostream& os, const Msg::Out::FolderContent& m)
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

#endif //MAGICPLAYER_MESSAGES_HPP
