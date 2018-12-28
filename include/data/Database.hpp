//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_DATABASE_HPP
#define MAGICPLAYER_DATABASE_HPP

#include "data/Artist.hpp"

#include <filesystem>
#include <cstdint>

namespace data
{
	struct Database final
	{
		std::uint64_t id;
		std::vector<std::filesystem::path> sources;

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
