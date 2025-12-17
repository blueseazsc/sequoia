#include "logger.h"

#include <spdlog/async.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/ansicolor_sink.h>

#include <iostream>

#define LOG_SECTION_NAME "SEQUOIA"
#define LOG_LEVEL_NAMES { "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL", "OFF" }

namespace sequoia::utility::log {

std::atomic<int32_t> Logger::default_logger_index_{0};
std::mutex Logger::default_logger_mutex_;
std::shared_ptr<Logger> Logger::default_logger_[Logger::DEFAULT_LOGGER_SIZE] = {nullptr, nullptr};

namespace internal {

static const std::string s_level_string_views[] LOG_LEVEL_NAMES;

class LogLevelFlag : public spdlog::custom_flag_formatter
{
public:
    void format(const spdlog::details::log_msg & msg, const std::tm &, spdlog::memory_buf_t &dest) override
    {
		const std::string &level_name = s_level_string_views[msg.level];
        dest.append(level_name.data(), level_name.data() + level_name.size());
    }

    std::unique_ptr<custom_flag_formatter> clone() const override
    {
        return spdlog::details::make_unique<LogLevelFlag>();
    }
};

void err_handler(const std::string& msg) {
    std::cerr << "*** Custom log error handler: " << msg << " ***" << std::endl;
}

template <typename SINK>
void sink_set_formatter(SINK sink) {
    std::string pattern = "[%Y-%m-%d %T.%e] [%t] [%^%V%$] [%n] %v";
    auto formatter = std::make_unique<spdlog::pattern_formatter>();
    formatter->add_flag<LogLevelFlag>('V').set_pattern(pattern);
    sink->set_formatter(std::move(formatter));
}

std::shared_ptr<spdlog::logger> create_spdlog(const std::string& section) {
    // Default thread pool settings can be modified *before* creating the async
    // logger: create global thread pool if not already exists..
    {
        std::lock_guard<std::recursive_mutex> tp_lock(
            spdlog::details::registry::instance().tp_mutex());
        if (nullptr == spdlog::thread_pool()) {
            spdlog::init_thread_pool(8192, 1);
        }
    }

    std::vector<spdlog::sink_ptr> sinks;
    auto console_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();

    sink_set_formatter(console_sink);
    sinks.push_back(console_sink);

    // create and register log
    auto logger = std::make_shared<spdlog::async_logger>(
        section, sinks.begin(), sinks.end(), spdlog::thread_pool(),
        spdlog::async_overflow_policy::block);
    logger->set_level(spdlog::level::level_enum::info);
    logger->flush_on(spdlog::level::level_enum::info);
    logger->set_error_handler(internal::err_handler);
    spdlog::register_logger(logger);

    char buf[32];
    std::time_t t =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm* local_time = std::localtime(&t);
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S (%z)", local_time);
    logger->warn("Log Info: time:{}", std::string(buf));
    return logger;
}
}  // namespace internal


Logger::Logger(const std::string& section, PrivateConstructor *)
    : section_{std::move(section)} {
    internal_logger_ = spdlog::get(section_);

    if (internal_logger_ == nullptr) {
		try {
            internal_logger_ = internal::create_spdlog(section_);
		}
		catch (const std::exception &ex) {
			internal::err_handler(ex.what());
		}
    }
}

std::shared_ptr<Logger> Logger::newLogger(const std::string &module_name) {
    return std::make_shared<Logger>(module_name,
                                    static_cast<PrivateConstructor *>(nullptr));
}

std::shared_ptr<Logger> Logger::defaultLogger() {
	return defaultLogger(LOG_SECTION_NAME);
}

std::shared_ptr<Logger> Logger::defaultLogger(const std::string &section) {
	int32_t current_index = default_logger_index_; 
	std::shared_ptr<Logger>& ret = default_logger_[current_index];
	if (ret)
		return ret;
	std::lock_guard<std::mutex> guard(default_logger_mutex_);
	if (!ret)
		ret = newLogger(section);
    return ret;
}

void Logger::shutdown() { 
	int32_t current_index = default_logger_index_; 
	int32_t cooldown_index = (current_index + 1) % DEFAULT_LOGGER_SIZE;
	default_logger_[cooldown_index].reset();
	spdlog::shutdown(); 

	// multi thread may new logger after this, 
	// so clean before ( shutdown before )
	default_logger_index_ = cooldown_index;
}

} // namespace sequoia::utility::log