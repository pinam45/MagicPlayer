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

#include <string>
#include <variant>

namespace Msg{

	namespace In{

		struct Close{
			//TODO: save?
		};

		struct Load{
			std::string path;

			explicit Load(std::string path);
		};

		struct Control{
			enum Action{
				PLAY,
				PAUSE,
				STOP
			};
			Action action;

			explicit Control(Action action);
		};

		struct Volume{
			bool muted;
			float volume;

			Volume(bool muted, float volume);
		};

		struct MusicOffset{
			float seconds;

			explicit MusicOffset(float seconds);
		};

		struct RequestMusicOffset{
			//TODO: general Request message with enum?
		};
	}

	namespace Out{

		struct MusicOffset{
			float seconds;

			explicit MusicOffset(float seconds);
		};

		struct MusicInfo{
			bool valid;
			float durationSeconds;

			MusicInfo(bool valid, float durationSeconds);
		};
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


#endif //MAGICPLAYER_MESSAGES_HPP
