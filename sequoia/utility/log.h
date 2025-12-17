#pragma once

#include "logger.h"

#define LOG_HEX(format, bin_to_hex) \
    ::sequoia::utility::log::Logger::defaultLogger()->hex(format, bin_to_hex);
#define LOG_TRACE(format, ...) \
    ::sequoia::utility::log::Logger::defaultLogger()->trace(format, ##__VA_ARGS__);
#define LOG_DEBUG(format, ...) \
    ::sequoia::utility::log::Logger::defaultLogger()->debug(format, ##__VA_ARGS__);
#define LOG_INFO(format, ...) \
    ::sequoia::utility::log::Logger::defaultLogger()->info(format, ##__VA_ARGS__);
#define LOG_WARN(format, ...) \
    ::sequoia::utility::log::Logger::defaultLogger()->warn(format, ##__VA_ARGS__);
#define LOG_ERROR(format, ...) \
    ::sequoia::utility::log::Logger::defaultLogger()->error(format, ##__VA_ARGS__);
#define LOG_FATAL(format, ...) \
    ::sequoia::utility::log::Logger::defaultLogger()->fatal(format, ##__VA_ARGS__);
#define LOG_ASSERT(condition, format, ...) \
    ::sequoia::utility::log::Logger::defaultLogger()->runtime_assert(condition, format, ##__VA_ARGS__);

