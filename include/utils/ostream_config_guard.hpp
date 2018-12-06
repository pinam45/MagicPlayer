//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_OSTREAM_CONFIG_GUARD_HPP
#define MAGICPLAYER_OSTREAM_CONFIG_GUARD_HPP

#include <ostream>

template<typename... Config>
class ostream_config_guard final
{
public:
	explicit ostream_config_guard(std::ostream& os, Config&&... configs);
	~ostream_config_guard();

private:
	std::ostream& os;
	std::ios_base::fmtflags flags;
};

template<typename... Config>
ostream_config_guard<Config...>::ostream_config_guard(std::ostream& os_, Config&&... configs)
  : os(os_), flags(os_.flags())
{
	(os << ... << std::forward<Config>(configs));
}

template<typename... Config>
ostream_config_guard<Config...>::~ostream_config_guard()
{
	os.flags(flags);
}

#endif //MAGICPLAYER_OSTREAM_CONFIG_GUARD_HPP
