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
#include "utils/path_utils.hpp"
#include "model/PathInfo.hpp"
#include "data/Database.hpp"
#include "data/Settings.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <iostream>
#include <iomanip>
#include <string>
#include <variant>

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
			utf8_path path;

			explicit Open(utf8_path path);
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

		struct Settings
		{
			data::Settings settings;
			bool save;

			explicit Settings(data::Settings settings, bool save = true);
		};
		std::ostream& operator<<(std::ostream& os, const Settings& m);

		struct RequestDatabase
		{
			bool generate_new;

			explicit RequestDatabase(bool generate_new = false);
		};
		std::ostream& operator<<(std::ostream& os, const RequestDatabase& m);

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
			utf8_path path;
			std::vector<PathInfo> content;

			explicit FolderContent(utf8_path path, const std::vector<PathInfo>& content = {});
			FolderContent(utf8_path path, std::vector<PathInfo>&& content);
		};
		std::ostream& operator<<(std::ostream& os, const FolderContent& m);

		struct Database
		{
			std::shared_ptr<const data::Database> database;

			explicit Database(std::shared_ptr<const data::Database> database);
		};
		std::ostream& operator<<(std::ostream& os, const Database& m);

		struct Settings
		{
			data::Settings settings;

			explicit Settings(data::Settings settings);
		};
		std::ostream& operator<<(std::ostream& os, const Settings& m);
	} // namespace Out

	struct Com final
	{
		typedef std::variant<In::Close,
		                     In::Open,
		                     In::Control,
		                     In::Volume,
		                     In::MusicOffset,
		                     In::RequestMusicOffset,
		                     In::Settings,
		                     In::RequestDatabase,
		                     In::InnerTaskEnded>
		  InMessage;
		typedef std::variant<Out::MusicOffset,
		                     Out::MusicInfo,
		                     Out::FolderContent,
		                     Out::Database,
		                     Out::Settings>
		  OutMessage;
		shared_queue<InMessage> in;
		shared_queue<OutMessage, true> out;

		template<typename Message, typename... Args>
		void sendInMessage(Args&&... args);

		template<typename Message, typename... Args>
		void sendOutMessage(Args&&... args);
	};

	// Com proxy for sending messages
	struct Sender final
	{
		explicit Sender(Com& com) noexcept;

		template<typename Message, typename... Args>
		void sendInMessage(Args&&... args);

		template<typename Message, typename... Args>
		void sendOutMessage(Args&&... args);

	private:
		Com& m_com;
	};
} // namespace Msg

template<typename Message, typename... Args>
void Msg::Com::sendInMessage(Args&&... args)
{
	in.emplace_back(std::in_place_type_t<Message>{}, std::forward<Args>(args)...);
}

template<typename Message, typename... Args>
void Msg::Com::sendOutMessage(Args&&... args)
{
	out.emplace_back(std::in_place_type_t<Message>{}, std::forward<Args>(args)...);
}

template<typename Message, typename... Args>
void Msg::Sender::sendInMessage(Args&&... args)
{
	m_com.sendInMessage<Message>(std::forward<Args>(args)...);
}

template<typename Message, typename... Args>
void Msg::Sender::sendOutMessage(Args&&... args)
{
	m_com.sendOutMessage<Message>(std::forward<Args>(args)...);
}

#endif //MAGICPLAYER_MESSAGES_HPP
