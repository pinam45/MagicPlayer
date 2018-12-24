//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "utils/IdGenerator.hpp"

#ifndef NDEBUG
#	include <utils/log.hpp>
#	include <spdlog/spdlog.h>
#endif

void IdGenerator::lock()
{
	m_mutex.lock();
#ifndef NDEBUG
	if(m_debug_locked)
	{
		spdlog::get(GENERAL_LOGGER_NAME)->error("IdGenerator: locked twice?");
	}
	m_debug_locked = true;
#endif
}

void IdGenerator::unlock() noexcept
{
#ifndef NDEBUG
	if(!m_debug_locked)
	{
		spdlog::get(GENERAL_LOGGER_NAME)->error("IdGenerator: unlocked while not locked");
	}
	m_debug_locked = false;
#endif
	m_mutex.unlock();
}

std::uint64_t IdGenerator::next_id() noexcept
{
#ifndef NDEBUG
	if(!m_debug_locked)
	{
		spdlog::get(GENERAL_LOGGER_NAME)->error("IdGenerator: used while not locked");
		m_mutex.unlock();
	}
#endif
	return m_next_id++;
}
