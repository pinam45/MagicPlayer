//
// Copyright (c) 2019 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_DEPENDENT_FALSE_HPP
#define MAGICPLAYER_DEPENDENT_FALSE_HPP

#include <type_traits>

template<typename T>
struct dependent_false : public std::false_type
{
};

template <typename T>
inline constexpr bool dependent_false_v = dependent_false<T>::value;

#endif //MAGICPLAYER_DEPENDENT_FALSE_HPP
