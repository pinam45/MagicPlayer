//
// Copyright (c) 2018 Maxime Pinard
//
// Distributed under the MIT license
// See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT
//
#ifndef MAGICPLAYER_LOG_HPP
#define MAGICPLAYER_LOG_HPP


constexpr const char* GENERAL_LOGGER_NAME = "general";
constexpr const char* LOGIC_LOGGER_NAME = "logic";
constexpr const char* VIEW_LOGGER_NAME = "view";

constexpr const char* LOGGERS_NAMES[] = {
  GENERAL_LOGGER_NAME,
  LOGIC_LOGGER_NAME,
  VIEW_LOGGER_NAME
};

bool init_logger();


#endif //MAGICPLAYER_LOG_HPP
