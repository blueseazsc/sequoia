#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <sequoia/utils/log/log.h>
#include <string>

using namespace sequoia::utils::log;

int PrintAll() {
	std::string str = "str_test!";
	auto logger = Logger::defaultLogger("Self");
	logger->info("self_logger! {}", str.c_str());
	logger->info("self_logger! {}", 5);
	Logger::shutdown();

	LOG_TRACE("log_test! {}", str.c_str());
	LOG_DEBUG("log_test! {}", 5);
	LOG_INFO("log_test!");
	LOG_WARN("log_test!");
	LOG_ERROR("log_test!");
	LOG_FATAL("log_test!");
	Logger::defaultLogger()->set_level(LogLevel::info);
	Logger::defaultLogger()->flush_on(LogLevel::error);
	LOG_TRACE("log_test_after_set_level! {}", str.c_str());
	LOG_TRACE("log_test_after_set_level! {}", 5);
	LOG_INFO("log_test_after_set_level! {}", 5);
	LOG_INFO("log_test_after_set_level! {}", str.c_str());
	Logger::shutdown();
	return 0;
}

TEST_CASE("Log Test") {
    CHECK(PrintAll() == 0);
}
