//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_DATABASE_HPP
#define MAGICPLAYER_DATABASE_HPP

#include "utils/path_utils.hpp"
#include "data/Artist.hpp"

#include <cstdint>
#include <chrono>

namespace data
{
	struct Database final
	{
		std::uint64_t id;
		std::vector<utf8_path> sources;
		std::chrono::system_clock::time_point generation_date;

		std::vector<Artist> artists;

		Database(const Database&) = delete;
		Database& operator=(const Database&) = delete;

		Database(Database&&) noexcept = default;
		Database& operator=(Database&&) noexcept = default;

		~Database() noexcept = default;

		std::vector<const Music*> findMusics(std::uint64_t id) const noexcept;

	private:
		friend class DataManager;

		Database() noexcept;
	};
} // namespace data

#endif //MAGICPLAYER_DATABASE_HPP
