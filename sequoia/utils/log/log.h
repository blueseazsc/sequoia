#pragma once

#include "logger.h"
#include <fmt/format.h>

#define LOG_HEX(format, bin_to_hex) \
    ::sequoia::utils::log::Logger::defaultLogger()->hex(format, bin_to_hex);
#define LOG_TRACE(format, ...) \
    ::sequoia::utils::log::Logger::defaultLogger()->trace(format, ##__VA_ARGS__);
#define LOG_DEBUG(format, ...) \
    ::sequoia::utils::log::Logger::defaultLogger()->debug(format, ##__VA_ARGS__);
#define LOG_INFO(format, ...) \
    ::sequoia::utils::log::Logger::defaultLogger()->info(format, ##__VA_ARGS__);
#define LOG_WARN(format, ...) \
    ::sequoia::utils::log::Logger::defaultLogger()->warn(format, ##__VA_ARGS__);
#define LOG_ERROR(format, ...) \
    ::sequoia::utils::log::Logger::defaultLogger()->error(format, ##__VA_ARGS__);
#define LOG_FATAL(format, ...) \
    ::sequoia::utils::log::Logger::defaultLogger()->fatal(format, ##__VA_ARGS__);
#define LOG_ASSERT(condition, format, ...) \
    ::sequoia::utils::log::Logger::defaultLogger()->runtime_assert(condition, format, ##__VA_ARGS__);

// C++20: 异常宏
#define SEQUOIA_CHECK_THROW(condition, exception_type, format, ...) \
    do { \
        if (!(condition)) { \
            auto __msg = fmt::format(fmt::runtime(std::string{format}), ##__VA_ARGS__); \
            LOG_ERROR("{}", __msg); \
            throw exception_type(__msg); \
        } \
    } while (false)

#define SEQUOIA_THROW_EXCEPTION(exception_type, format, ...) \
    do { \
        auto __msg = fmt::format(fmt::runtime(std::string{format}), ##__VA_ARGS__); \
        LOG_ERROR("{}", __msg); \
        throw exception_type(__msg); \
    } while (false)

#define SEQUOIA_IF_RETURN(condition, return_value) \
    do { \
        if (condition) { \
            return return_value; \
        } \
    } while (false)

