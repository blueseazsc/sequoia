#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/bin_to_hex.h>

#include <string>
#include <string_view>
#include <mutex>
#include <array>
#include <source_location>
#include <concepts>

namespace sequoia::utils::log {

// C++20 Concept: 约束可格式化的类型，排除 nullptr_t
template<typename T>
concept Loggable = !std::is_same_v<std::remove_cvref_t<T>, std::nullptr_t>;

// C++20 Concept: 约束指针类型
template<typename T>
concept LoggablePointer = std::is_pointer_v<T> && Loggable<std::remove_pointer_t<T>>;

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
    explicit Logger(std::string_view section, PrivateConstructor *);
    [[nodiscard]] std::string_view section() const noexcept { return section_; }

    // 十六进制输出
    template <typename Arg1>
    void hex(std::string_view fmt, const Arg1 &arg1) {
        log(LogLevel::trace, fmt, spdlog::to_hex(arg1));
    }

    // trace 级别日志 - 使用 C++20 concepts 简化重载
    template <Loggable Arg1, Loggable... Args>
    void trace(std::string_view fmt, const Arg1 &arg1, const Args &... args) {
        log(LogLevel::trace, fmt, arg1, args...);
    }

    template <Loggable Arg1>
        requires (!std::convertible_to<Arg1, std::string_view>)
    void trace(const Arg1 &arg1) {
        trace("{}", arg1);
    }

    template <LoggablePointer Arg1>
    void trace(Arg1 arg1) {
        if (arg1) trace("{}", *arg1);
    }

    // debug 级别日志
    template <Loggable Arg1, Loggable... Args>
    void debug(std::string_view fmt, const Arg1 &arg1, const Args &... args) {
        log(LogLevel::debug, fmt, arg1, args...);
    }

    template <Loggable Arg1>
        requires (!std::convertible_to<Arg1, std::string_view>)
    void debug(const Arg1 &arg1) {
        debug("{}", arg1);
    }

    template <LoggablePointer Arg1>
    void debug(Arg1 arg1) {
        if (arg1) debug("{}", *arg1);
    }

    // info 级别日志
    template <Loggable Arg1, Loggable... Args>
    void info(std::string_view fmt, const Arg1 &arg1, const Args &... args) {
        log(LogLevel::info, fmt, arg1, args...);
    }

    template <Loggable Arg1>
        requires (!std::convertible_to<Arg1, std::string_view>)
    void info(const Arg1 &arg1) {
        info("{}", arg1);
    }

    template <LoggablePointer Arg1>
    void info(Arg1 arg1) {
        if (arg1) info("{}", *arg1);
    }

    // warn 级别日志
    template <Loggable Arg1, Loggable... Args>
    void warn(std::string_view fmt, const Arg1 &arg1, const Args &... args) {
        log(LogLevel::warn, fmt, arg1, args...);
    }

    template <Loggable Arg1>
        requires (!std::convertible_to<Arg1, std::string_view>)
    void warn(const Arg1 &arg1) {
        warn("{}", arg1);
    }

    template <LoggablePointer Arg1>
    void warn(Arg1 arg1) {
        if (arg1) warn("{}", *arg1);
    }

    // error 级别日志
    template <Loggable Arg1, Loggable... Args>
    void error(std::string_view fmt, const Arg1 &arg1, const Args &... args) {
        log(LogLevel::error, fmt, arg1, args...);
    }

    template <Loggable Arg1>
        requires (!std::convertible_to<Arg1, std::string_view>)
    void error(const Arg1 &arg1) {
        error("{}", arg1);
    }

    template <LoggablePointer Arg1>
    void error(Arg1 arg1) {
        if (arg1) error("{}", *arg1);
    }

    // fatal 级别日志
    template <Loggable Arg1, Loggable... Args>
    void fatal(std::string_view fmt, const Arg1 &arg1, const Args &... args) {
        log(LogLevel::critical, fmt, arg1, args...);
    }

    template <Loggable Arg1>
        requires (!std::convertible_to<Arg1, std::string_view>)
    void fatal(const Arg1 &arg1) {
        fatal("{}", arg1);
    }

    template <LoggablePointer Arg1>
    void fatal(Arg1 arg1) {
        if (arg1) fatal("{}", *arg1);
    }

    // 运行时断言 - 格式化版本（优先级较低）
    template <Loggable Arg1, typename... Args>
        requires (sizeof...(Args) > 0)
    void runtime_assert(bool condition, std::string_view fmt, const Arg1 &arg1, 
                       const Args &... args) {
        if (!condition) {
            log(LogLevel::critical, fmt, arg1, args...);
            shutdown();
            std::abort();
        }
    }

    // 简单消息版本（优先级较高）
    template <Loggable Arg1>
        requires (!std::convertible_to<Arg1, std::string_view> || 
                  std::same_as<std::remove_cvref_t<Arg1>, std::string> ||
                  std::same_as<std::remove_cvref_t<Arg1>, std::string_view>)
    void runtime_assert(bool condition, const Arg1 &arg1) {
        runtime_assert(condition, "{}", arg1);
    }

    // 字符串字面量特化
    void runtime_assert(bool condition, const char* msg) {
        if (!condition) {
            log(LogLevel::critical, "{}", msg);
            shutdown();
            std::abort();
        }
    }

    // 指针版本
    template <LoggablePointer Arg1>
    void runtime_assert(bool condition, Arg1 arg1) {
        if (arg1)
            runtime_assert(condition, *arg1);
        else
            runtime_assert(condition, "no assert message");
    }

    // C++20 source_location 版本 - 提供详细的调用位置信息
    template <Loggable Arg1, typename... Args>
    void runtime_assert_loc(bool condition, std::string_view fmt, const Arg1 &arg1, 
                           const Args &... args,
                           const std::source_location& loc = std::source_location::current()) {
        if (!condition) {
            internal_logger_->log(static_cast<spdlog::level::level_enum>(LogLevel::critical),
                                 "Assert failed at {}:{} in {}: {}",
                                 loc.file_name(), loc.line(), loc.function_name(),
                                 fmt::format(fmt::runtime(std::string{fmt}), arg1, args...));
            shutdown();
            std::abort();
        }
    }

    void set_level(const LogLevel level) noexcept {
        internal_logger_->set_level(
            static_cast<spdlog::level::level_enum>(level));
    }

    void flush_on(const LogLevel level) noexcept {
        internal_logger_->flush_on(
            static_cast<spdlog::level::level_enum>(level));
    }
public:
    [[nodiscard]] static std::shared_ptr<Logger> defaultLogger();
    [[nodiscard]] static std::shared_ptr<Logger> defaultLogger(std::string_view section);

    static void shutdown();
    
private:
    [[nodiscard]] static std::shared_ptr<Logger> newLogger(std::string_view module_name);

    template <typename Arg1, typename... Args>
    void log(const LogLevel level, std::string_view fmt, const Arg1 &arg1,
             const Args &... args) {
        if (!fmt.empty()) {
            internal_logger_->log(static_cast<spdlog::level::level_enum>(level),
                                 fmt::runtime(std::string{fmt}), arg1, args...);
        }
    }

private:
    /// @brief 用于区分不同模块
    std::string section_;
    /// @brief 底层 spdlog 实例
    std::shared_ptr<spdlog::logger> internal_logger_;

    /// @brief 默认 Logger 实例池（用于多线程安全重启）
    static constexpr size_t DEFAULT_LOGGER_SIZE = 2;
    static inline std::atomic<int32_t> default_logger_index_{0};
    static inline std::mutex default_logger_mutex_;
    static inline std::array<std::shared_ptr<Logger>, DEFAULT_LOGGER_SIZE> default_logger_{nullptr, nullptr};
};

// C++20 concepts 已经通过 Loggable concept 自动排除 nullptr_t，无需特化

using LoggerPtr = std::shared_ptr<Logger>;

class LoggerCloser {
public:
	~LoggerCloser() {
		Logger::shutdown();
	}
};

} // namespace sequoia::util::log