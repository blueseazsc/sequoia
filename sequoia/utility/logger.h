#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/bin_to_hex.h>

#include <string>
#include <mutex>

namespace sequoia::utility::log {

enum class LogLevel {
    trace = spdlog::level::level_enum::trace,
    debug = spdlog::level::level_enum::debug,
    info = spdlog::level::level_enum::info,
    warn = spdlog::level::level_enum::warn,
    error = spdlog::level::level_enum::err,
    critical = spdlog::level::level_enum::critical,
    off = spdlog::level::level_enum::off
};

class Logger {
private:
    struct PrivateConstructor;
public:
    explicit Logger(const std::string& section, PrivateConstructor *);
    std::string section() const { return section_; }

    template <typename Arg1>
    void hex(const char *fmt, const Arg1 &arg1) {
        log(LogLevel::trace, fmt, spdlog::to_hex(arg1));
    }

    template <typename Arg1, typename... Args>
    void trace(const char *fmt, const Arg1 &arg1, const Args &... args) {
        log(LogLevel::trace, fmt, arg1, args...);
    }

    template <typename Arg1>
    void trace(const Arg1 &arg1) {
        trace("{}", arg1);
    }

    template <typename Arg1>
    void trace(Arg1 *arg1) {
		if (arg1)
			trace("{}", arg1);
    }

    template <typename Arg1, typename... Args>
    void debug(const char *fmt, const Arg1 &arg1, const Args &... args) {
        log(LogLevel::debug, fmt, arg1, args...);
    }

    template <typename Arg1>
    void debug(const Arg1 &arg1) {
        debug("{}", arg1);
    }

    template <typename Arg1>
    void debug(Arg1 *arg1) {
		if (arg1)
			debug("{}", arg1);
    }

    template <typename Arg1, typename... Args>
    void info(const char *fmt, const Arg1 &arg1, const Args &... args) {
        log(LogLevel::info, fmt, arg1, args...);
    }

    template <typename Arg1>
    void info(const Arg1 &arg1) {
        info("{}", arg1);
    }

    template <typename Arg1>
    void info(Arg1 *arg1) {
		if (arg1)
			info("{}", arg1);
    }

    template <typename Arg1, typename... Args>
    void warn(const char *fmt, const Arg1 &arg1, const Args &... args) {
        log(LogLevel::warn, fmt, arg1, args...);
    }

    template <typename Arg1>
    void warn(const Arg1 &arg1) {
        warn("{}", arg1);
    }

    template <typename Arg1>
    void warn(Arg1 *arg1) {
		if (arg1)
			warn("{}", arg1);
    }

    template <typename Arg1, typename... Args>
    void error(const char *fmt, const Arg1 &arg1, const Args &... args) {
        log(LogLevel::error, fmt, arg1, args...);
    }

    template <typename Arg1>
    void error(const Arg1 &arg1) {
        error("{}", arg1);
    }

    template <typename Arg1>
    void error(Arg1 *arg1) {
		if (arg1)
			error("{}", arg1);
    }

    template <typename Arg1, typename... Args>
    void fatal(const char *fmt, const Arg1 &arg1, const Args &... args) {
        log(LogLevel::critical, fmt, arg1, args...);
    }

    template <typename Arg1>
    void fatal(const Arg1 &arg1) {
        fatal("{}", arg1);
    }

    template <typename Arg1>
    void fatal(Arg1 *arg1) {
		if (arg1)
			fatal("{}", arg1);
    }

    template <typename Arg1, typename... Args>
    void runtime_assert(bool condition, const char *fmt, const Arg1 &arg1, const Args &... args) {
		if (!condition) {
			log(LogLevel::critical, fmt, arg1, args...);
			shutdown();
			std::abort();
		}
    }

    template <typename Arg1>
    void runtime_assert(bool condition, const Arg1 &arg1) {
        runtime_assert(condition, "{}", arg1);
    }

    template <typename Arg1>
    void runtime_assert(bool condition, Arg1 *arg1) {
		if (arg1)
			runtime_assert(condition, "{}", arg1);
		else
			runtime_assert(condition, "{}", "no assert message");
    }

    void set_level(const LogLevel level) {
        internal_logger_->set_level(
            static_cast<spdlog::level::level_enum>(level));
    }

    void flush_on(const LogLevel level) {
        internal_logger_->flush_on(
            static_cast<spdlog::level::level_enum>(level));
    }
public:
    static std::shared_ptr<Logger> defaultLogger();
    static std::shared_ptr<Logger> defaultLogger(const std::string &section);

    static void shutdown();
private:
    static std::shared_ptr<Logger> newLogger(const std::string &module_name);

    template <typename Arg1, typename... Args>
    void log(const LogLevel level, const char *fmt, const Arg1 &arg1,
             const Args &... args) {
		if (fmt)
			internal_logger_->log(static_cast<spdlog::level::level_enum>(level),
					fmt, arg1, args...);
    }

private:
    /// @brief used to differentiate between modules
    std::string section_{""};
    /// @brief internal spdlog instance for supporting Logger
    std::shared_ptr<spdlog::logger> internal_logger_{nullptr};

    /// @brief default Logger for general use
	enum {
		DEFAULT_LOGGER_SIZE = 2,
	};
    static std::atomic<int32_t> default_logger_index_;
	static std::mutex default_logger_mutex_;
    static std::shared_ptr<Logger> default_logger_[DEFAULT_LOGGER_SIZE];
};

template <>
inline void Logger::trace<nullptr_t>(const nullptr_t &arg1) {
}

template <>
inline void Logger::debug<nullptr_t>(const nullptr_t &arg1) {
}

template <>
inline void Logger::info<nullptr_t>(const nullptr_t &arg1) {
}

template <>
inline void Logger::warn<nullptr_t>(const nullptr_t &arg1) {
}

template <>
inline void Logger::error<nullptr_t>(const nullptr_t &arg1) {
}

template <>
inline void Logger::fatal<nullptr_t>(const nullptr_t &arg1) {
}

template <>
inline void Logger::runtime_assert<nullptr_t>(bool condition, const nullptr_t &arg1) {
	runtime_assert(condition, "{}", "no assert message");
}

using LoggerPtr = std::shared_ptr<Logger>;

class LoggerCloser {
public:
	~LoggerCloser() {
		Logger::shutdown();
	}
};

} // namespace sequoia::util::log