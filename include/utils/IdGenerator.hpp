//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_IDGENERATOR_HPP
#define MAGICPLAYER_IDGENERATOR_HPP

#include <cstdint>
#include <mutex>

class IdGenerator
{
public:
	IdGenerator() noexcept = default;

	IdGenerator(const IdGenerator&) = delete;
	IdGenerator& operator=(const IdGenerator&) = delete;

	IdGenerator(IdGenerator&&) = delete;
	IdGenerator& operator=(IdGenerator&&) = delete;

	~IdGenerator() noexcept = default;

	void lock();
	void unlock() noexcept;

	std::uint64_t next_id() noexcept;

private:
	std::mutex m_mutex;
	std::uint64_t m_next_id = 1; // keep 0 for invalid id
#ifndef NDEBUG
	bool m_debug_locked = false;
#endif
};

#endif //MAGICPLAYER_IDGENERATOR_HPP
