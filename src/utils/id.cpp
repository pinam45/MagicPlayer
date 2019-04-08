//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#include "utils/id.hpp"

#ifndef NDEBUG
#	include <utils/log.hpp>
#	include <spdlog/spdlog.h>
#endif

void id::Generator::lock()
{
	m_mutex.lock();
#ifndef NDEBUG
	if(m_debug_locked)
	{
		spdlog::get(GENERAL_LOGGER_NAME)->error("id::Generator: locked twice?");
	}
	m_debug_locked = true;
#endif
}

void id::Generator::unlock() noexcept
{
#ifndef NDEBUG
	if(!m_debug_locked)
	{
		spdlog::get(GENERAL_LOGGER_NAME)->error("id::Generator: unlocked while not locked");
	}
	m_debug_locked = false;
#endif
	m_mutex.unlock();
}

id::type id::Generator::next_id() noexcept
{
#ifndef NDEBUG
	if(!m_debug_locked)
	{
		spdlog::get(GENERAL_LOGGER_NAME)->error("id::Generator: used while not locked");
		m_mutex.unlock();
	}
#endif
	return m_next_id++;
}
