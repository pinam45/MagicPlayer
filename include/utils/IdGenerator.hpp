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

namespace id
{
	typedef std::uint64_t type;
	constexpr type INVALID = 0;

	class Generator
	{
	public:
		Generator() noexcept = default;

		Generator(const Generator&) = delete;
		Generator& operator=(const Generator&) = delete;

		Generator(Generator&&) = delete;
		Generator& operator=(Generator&&) = delete;

		~Generator() noexcept = default;

		void lock();
		void unlock() noexcept;

		type next_id() noexcept;

	private:
		std::mutex m_mutex;
		type m_next_id = INVALID + 1;
#ifndef NDEBUG
		bool m_debug_locked = false;
#endif
	};
} // namespace id

#endif //MAGICPLAYER_IDGENERATOR_HPP
