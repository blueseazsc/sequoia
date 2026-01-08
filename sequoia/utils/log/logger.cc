#include "logger.h"

#include <spdlog/async.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/ansicolor_sink.h>

#include <iostream>

namespace sequoia::utils::log {

// C++20: 使用 constexpr 字符串常量
constexpr std::string_view LOG_SECTION_NAME = "SEQUOIA";

namespace internal {

// C++20: 使用 constexpr std::array 替代 C 风格数组和宏
constexpr std::array<std::string_view, 7> LOG_LEVEL_NAMES = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL", "OFF"
};

// C++20: 自定义日志级别格式化器
class LogLevelFlag final : public spdlog::custom_flag_formatter
{
public:
    void format(const spdlog::details::log_msg& msg, const std::tm&, 
               spdlog::memory_buf_t& dest) override
    {
        const std::string_view level_name = LOG_LEVEL_NAMES[msg.level];
        dest.append(level_name.data(), level_name.data() + level_name.size());
    }

    [[nodiscard]] std::unique_ptr<custom_flag_formatter> clone() const override
    {
        return spdlog::details::make_unique<LogLevelFlag>();
    }
};

// C++20: 使用 string_view 提高性能
void err_handler(std::string_view msg) {
    std::cerr << "*** Custom log error handler: " << msg << " ***" << std::endl;
}

// C++20: 使用 concepts 约束模板参数
template <typename SINK>
    requires std::is_pointer_v<SINK> || requires(SINK s) { s->set_formatter(nullptr); }
void sink_set_formatter(SINK sink) {
    constexpr std::string_view pattern = "[%Y-%m-%d %T.%e] [%t] [%^%V%$] [%n] %v";
    auto formatter = std::make_unique<spdlog::pattern_formatter>();
    formatter->add_flag<LogLevelFlag>('V').set_pattern(std::string{pattern});
    sink->set_formatter(std::move(formatter));
}

[[nodiscard]] std::shared_ptr<spdlog::logger> create_spdlog(std::string_view section) {
    // C++20: 初始化全局线程池（如果尚未创建）
    {
        if (const std::lock_guard<std::recursive_mutex> tp_lock(
                spdlog::details::registry::instance().tp_mutex());
            spdlog::thread_pool() == nullptr) {
            spdlog::init_thread_pool(8192, 1);
        }
    }

    // C++20: 使用初始化列表直接构造 vector
    std::vector<spdlog::sink_ptr> sinks;
    auto console_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();

    sink_set_formatter(console_sink);
    sinks.push_back(console_sink);

    // 创建并注册异步 logger
    auto logger = std::make_shared<spdlog::async_logger>(
        std::string{section}, sinks.begin(), sinks.end(), 
        spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    
    logger->set_level(spdlog::level::level_enum::info);
    logger->flush_on(spdlog::level::level_enum::info);
    logger->set_error_handler([](const std::string& msg) { err_handler(msg); });
    spdlog::register_logger(logger);

    // C++20: 使用 chrono 和格式化输出时间
    const auto now = std::chrono::system_clock::now();
    const std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::array<char, 32> buf{};
    if (const std::tm* local_time = std::localtime(&t)) {
        std::strftime(buf.data(), buf.size(), "%Y-%m-%d %H:%M:%S (%z)", local_time);
        logger->warn("Log Info: time:{}", std::string_view{buf.data()});
    }
    
    return logger;
}
}  // namespace internal

// C++20: 使用 string_view 构造，避免不必要的字符串拷贝
Logger::Logger(std::string_view section, PrivateConstructor*)
    : section_{section} {
    internal_logger_ = spdlog::get(section_);

    if (internal_logger_ == nullptr) {
        try {
            internal_logger_ = internal::create_spdlog(section_);
        }
        catch (const std::exception& ex) {
            internal::err_handler(ex.what());
        }
    }
}

std::shared_ptr<Logger> Logger::newLogger(std::string_view module_name) {
    return std::make_shared<Logger>(module_name, nullptr);
}

std::shared_ptr<Logger> Logger::defaultLogger() {
    return defaultLogger(LOG_SECTION_NAME);
}

std::shared_ptr<Logger> Logger::defaultLogger(std::string_view section) {
    // C++20: 使用 load/store 显式原子操作
    const int32_t current_index = default_logger_index_.load(std::memory_order_acquire);
    std::shared_ptr<Logger>& ret = default_logger_[current_index];
    
    if (ret) {
        return ret;
    }
    
    // 双重检查锁定模式
    const std::lock_guard<std::mutex> guard(default_logger_mutex_);
    if (!ret) {
        ret = newLogger(section);
    }
    return ret;
}

void Logger::shutdown() {
    // C++20: 使用显式原子操作和更清晰的逻辑
    const int32_t current_index = default_logger_index_.load(std::memory_order_acquire);
    const int32_t cooldown_index = (current_index + 1) % DEFAULT_LOGGER_SIZE;
    
    default_logger_[cooldown_index].reset();
    spdlog::shutdown();

    // 多线程可能在此之后创建新的 logger
    // 因此先清理再切换索引（shutdown 在前）
    default_logger_index_.store(cooldown_index, std::memory_order_release);
}

} // namespace sequoia::utils::log