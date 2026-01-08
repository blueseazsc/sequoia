#include <sequoia/utils/log/log.h>
#include <thread>
#include <chrono>
#include <iostream>

using namespace sequoia::utils;

void func(int index) {
	log::Logger::defaultLogger()->set_level(log::LogLevel::trace);
	LOG_TRACE("{} log_test!", index * 10 + 0);
	LOG_DEBUG("{} log_test!", index * 10 + 1);
	LOG_INFO("{} log_test!", index * 10 + 2);
	LOG_WARN("{} log_test!", index * 10 + 3);
	LOG_ERROR("{} log_test!", index * 10 + 4);
	LOG_FATAL("{} log_test!", index * 10 + 5);
	LOG_TRACE(nullptr);
	LOG_DEBUG(nullptr);
	LOG_INFO(nullptr);
	LOG_WARN(nullptr);
	LOG_ERROR(nullptr);
	LOG_FATAL(nullptr);
	// if (index == 3)
	// 	Logger::shutdown();
#ifdef FRAMEWORK_SYSTEM_64 
	LOG_INFO("{} log_test! in 64", index * 10 + 6);
#else
	LOG_INFO("{} log_test! in 32", index * 10 + 6);
#endif
#ifdef FRAMEWORK_SYSTEM_XRLINUX
	LOG_INFO("{} log_test! in xrlinux", index * 10 + 11);
#else
	LOG_INFO("{} log_test! not in xrlinux", index * 10 + 11);
#endif
	log::Logger::defaultLogger()->set_level(log::LogLevel::info);
	LOG_TRACE("{} log_test_after_set_level!", index * 10 + 7);
	LOG_INFO("{} log_test_after_set_level!", index * 10 + 8);
	LOG_INFO("{} shutdown", index * 10 + 9);
	// Logger::shutdown();
}
void func_multi() {
	const int count = 10;

	std::thread th[count];

	for(int i = 0; i < count; ++i) {
		std::thread t(func, i);
		th[i].swap(t);
	}

	for(int i = 0; i < count; ++i) {
		th[i].join();
	}

	// DoZipFile("sdk.txt.zip", "sdk_2022-05-13.txt", false, 9);
}
void log_imp(const char * a) {
	LOG_INFO(a)
} 

int main() {
	log::LoggerCloser lc;
	log_imp("fda");
	log_imp(nullptr);
	func_multi();
	func_multi();
	func_multi();
	func_multi();
}
