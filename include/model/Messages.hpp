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

namespace Msg{

	namespace details{
		struct ostream_config_guard{
			std::ostream& os;
			std::ios_base::fmtflags flags;

			explicit ostream_config_guard(std::ostream& os);
			~ostream_config_guard();
		};
	}

	namespace In{

		struct Close{
			//TODO: save?
		};
		std::ostream& operator<<(std::ostream& os, const Close& m);

		struct Load{
			std::filesystem::path path;

			explicit Load(std::filesystem::path path);
		};
		std::ostream& operator<<(std::ostream& os, const Load& m);


		struct Control{
			enum Action{
				PLAY,
				PAUSE,
				STOP,
			};
			Action action;

			explicit Control(Action action);
		};
		std::ostream& operator<<(std::ostream& os, const Control::Action& a);
		std::ostream& operator<<(std::ostream& os, const Control& m);

		struct Volume{
			bool muted;
			float volume;

			Volume(bool muted, float volume);
		};
		std::ostream& operator<<(std::ostream& os, const Volume& m);

		struct MusicOffset{
			float seconds;

			explicit MusicOffset(float seconds);
		};
		std::ostream& operator<<(std::ostream& os, const MusicOffset& m);

		struct RequestMusicOffset{
			//TODO: general Request message with enum?
		};
		std::ostream& operator<<(std::ostream& os, const RequestMusicOffset& m);
	}

	namespace Out{

		struct MusicOffset{
			float seconds;

			explicit MusicOffset(float seconds);
		};
		std::ostream& operator<<(std::ostream& os, const MusicOffset& m);

		struct MusicInfo{
			bool valid;
			float durationSeconds;

			MusicInfo(bool valid, float durationSeconds);
		};
		std::ostream& operator<<(std::ostream& os, const MusicInfo& m);
	}

	struct Com{
		typedef std::variant<
		  In::Close,
		  In::Load,
		  In::Control,
		  In::Volume,
		  In::MusicOffset,
		  In::RequestMusicOffset
		> InMessage;
		typedef std::variant<
		  Out::MusicOffset,
		  Out::MusicInfo
		> OutMessage;
		shared_queue<InMessage> in;
		shared_queue<OutMessage, true> out;
	};
}

inline std::ostream& Msg::In::operator<<(std::ostream& os, [[maybe_unused]] const Msg::In::Close& m){
	return os << "Close{}";
}

inline std::ostream& Msg::In::operator<<(std::ostream& os, const Msg::In::Load& m){
	return os << "Load{"
	          << "path: "<< m.path
	          << "}";
}

inline std::ostream& Msg::In::operator<<(std::ostream& os, const Msg::In::Control::Action& a){
	constexpr const char* ACTION_STR[] = {
	  "PLAY",
	  "PAUSE",
	  "STOP",
	};
	return os << ACTION_STR[a];
}

inline std::ostream& Msg::In::operator<<(std::ostream& os, const Msg::In::Control& m){
	return os << "Control{"
	          << "action: "<< m.action
	          << "}";
}

inline std::ostream& Msg::In::operator<<(std::ostream& os, const Msg::In::Volume& m){
	Msg::details::ostream_config_guard guard(os);
	return os << "Volume{"
	          << "muted: "<< m.muted << ","
	          << "volume:" << m.volume
	          << "}";
}

inline std::ostream& Msg::In::operator<<(std::ostream& os, const Msg::In::MusicOffset& m){
	return os << "MusicOffset{"
	          << "seconds: "<< m.seconds
	          << "}";
}

inline std::ostream& Msg::In::operator<<(std::ostream& os, [[maybe_unused]] const Msg::In::RequestMusicOffset& m){
	return os << "RequestMusicOffset{}";
}

inline std::ostream& Msg::Out::operator<<(std::ostream& os, const Msg::Out::MusicOffset& m){
	return os << "MusicOffset{"
	          << "seconds: "<< m.seconds
	          << "}";
}

inline std::ostream& Msg::Out::operator<<(std::ostream& os, const Msg::Out::MusicInfo& m){
	Msg::details::ostream_config_guard guard(os);
	return os << "MusicInfo{"
	          << "valid: "<< m.valid << ","
	          << "durationSeconds:" << m.durationSeconds
	          << "}";
}


#endif //MAGICPLAYER_MESSAGES_HPP
