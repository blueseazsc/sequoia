#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <sequoia/utils/any_to_string.h>
#include <any>
#include <string>
#include <cmath>

using namespace sequoia::utils;

// 定义一个自定义类型用于测试
class CustomType {
public:
    CustomType(int val) : value(val) {}
    explicit CustomType(const std::string& str) : value(std::stoi(str)) {}
    
    std::string to_string() const {
        return "CustomType(" + std::to_string(value) + ")";
    }
    
    int getValue() const { return value; }
    
private:
    int value;
};

TEST_CASE("any_to_string - 整型类型") {
    SUBCASE("int8_t") {
        std::any data = static_cast<int8_t>(42);
        CHECK(any_to_string<int8_t>(data) == "42");
    }
    
    SUBCASE("uint8_t") {
        std::any data = static_cast<uint8_t>(255);
        CHECK(any_to_string<uint8_t>(data) == "255");
    }
    
    SUBCASE("int16_t") {
        std::any data = static_cast<int16_t>(-1234);
        CHECK(any_to_string<int16_t>(data) == "-1234");
    }
    
    SUBCASE("uint16_t") {
        std::any data = static_cast<uint16_t>(65535);
        CHECK(any_to_string<uint16_t>(data) == "65535");
    }
    
    SUBCASE("int32_t") {
        std::any data = static_cast<int32_t>(-123456);
        CHECK(any_to_string<int32_t>(data) == "-123456");
    }
    
    SUBCASE("uint32_t") {
        std::any data = static_cast<uint32_t>(4294967295U);
        CHECK(any_to_string<uint32_t>(data) == "4294967295");
    }
    
    SUBCASE("int64_t") {
        std::any data = static_cast<int64_t>(-9223372036854775807LL);
        CHECK(any_to_string<int64_t>(data) == "-9223372036854775807");
    }
    
    SUBCASE("uint64_t") {
        std::any data = static_cast<uint64_t>(18446744073709551615ULL);
        CHECK(any_to_string<uint64_t>(data) == "18446744073709551615");
    }
}

TEST_CASE("any_to_string - 浮点类型") {
    SUBCASE("float") {
        std::any data = 3.14f;
        std::string result = any_to_string<float>(data);
        CHECK(result.substr(0, 4) == "3.14");
    }
    
    SUBCASE("double") {
        std::any data = 3.141592653589793;
        std::string result = any_to_string<double>(data);
        CHECK(result.substr(0, 4) == "3.14");
    }
    
    SUBCASE("long double") {
        std::any data = static_cast<long double>(2.718281828459045);
        std::string result = any_to_string<long double>(data);
        CHECK(result.substr(0, 4) == "2.71");
    }
    
    SUBCASE("负数浮点") {
        std::any data = -123.456;
        std::string result = any_to_string<double>(data);
        CHECK(result.substr(0, 7) == "-123.45");
    }
}

TEST_CASE("any_to_string - 自定义类型") {
    SUBCASE("调用对象的 to_string 方法") {
        CustomType obj(42);
        std::any data = obj;
        CHECK(any_to_string<CustomType>(data) == "CustomType(42)");
    }
    
    SUBCASE("负数") {
        CustomType obj(-100);
        std::any data = obj;
        CHECK(any_to_string<CustomType>(data) == "CustomType(-100)");
    }
}

TEST_CASE("string_to_any - 整型类型") {
    SUBCASE("bool - true") {
        std::any result = string_to_any<bool>("true");
        CHECK(std::any_cast<bool>(result) == true);
    }
    
    SUBCASE("bool - 1") {
        std::any result = string_to_any<bool>("1");
        CHECK(std::any_cast<bool>(result) == true);
    }
    
    SUBCASE("bool - false") {
        std::any result = string_to_any<bool>("false");
        CHECK(std::any_cast<bool>(result) == false);
    }
    
    SUBCASE("int8_t") {
        std::any result = string_to_any<int8_t>("42");
        CHECK(std::any_cast<int8_t>(result) == 42);
    }
    
    SUBCASE("uint8_t") {
        std::any result = string_to_any<uint8_t>("255");
        CHECK(std::any_cast<uint8_t>(result) == 255);
    }
    
    SUBCASE("int16_t") {
        std::any result = string_to_any<int16_t>("-1234");
        CHECK(std::any_cast<int16_t>(result) == -1234);
    }
    
    SUBCASE("uint16_t") {
        std::any result = string_to_any<uint16_t>("65535");
        CHECK(std::any_cast<uint16_t>(result) == 65535);
    }
    
    SUBCASE("int32_t") {
        std::any result = string_to_any<int32_t>("-123456");
        CHECK(std::any_cast<int32_t>(result) == -123456);
    }
    
    SUBCASE("uint32_t") {
        std::any result = string_to_any<uint32_t>("4294967295");
        CHECK(std::any_cast<uint32_t>(result) == 4294967295U);
    }
    
    SUBCASE("int64_t") {
        std::any result = string_to_any<int64_t>("-9223372036854775807");
        CHECK(std::any_cast<int64_t>(result) == -9223372036854775807LL);
    }
    
    SUBCASE("uint64_t") {
        std::any result = string_to_any<uint64_t>("18446744073709551615");
        CHECK(std::any_cast<uint64_t>(result) == 18446744073709551615ULL);
    }
}

TEST_CASE("string_to_any - 浮点类型") {
    SUBCASE("float") {
        std::any result = string_to_any<float>("3.14");
        CHECK(std::abs(std::any_cast<float>(result) - 3.14f) < 0.001f);
    }
    
    SUBCASE("double") {
        std::any result = string_to_any<double>("3.141592653589793");
        CHECK(std::abs(std::any_cast<double>(result) - 3.141592653589793) < 0.0000001);
    }
    
    SUBCASE("long double") {
        std::any result = string_to_any<long double>("2.718281828459045");
        CHECK(std::abs(std::any_cast<long double>(result) - 2.718281828459045L) < 0.0000001L);
    }
    
    SUBCASE("负数浮点") {
        std::any result = string_to_any<double>("-123.456");
        CHECK(std::abs(std::any_cast<double>(result) - (-123.456)) < 0.001);
    }
    
    SUBCASE("科学计数法") {
        std::any result = string_to_any<double>("1.23e-4");
        CHECK(std::abs(std::any_cast<double>(result) - 0.000123) < 0.0000001);
    }
}

TEST_CASE("string_to_any - std::string") {
    SUBCASE("普通字符串") {
        std::any result = string_to_any<std::string>("Hello World");
        CHECK(std::any_cast<std::string>(result) == "Hello World");
    }
    
    SUBCASE("空字符串") {
        std::any result = string_to_any<std::string>("");
        CHECK(std::any_cast<std::string>(result) == "");
    }
    
    SUBCASE("包含特殊字符") {
        std::any result = string_to_any<std::string>("Test!@#$%^&*()");
        CHECK(std::any_cast<std::string>(result) == "Test!@#$%^&*()");
    }
}

TEST_CASE("string_to_any - 自定义类型") {
    SUBCASE("通过构造函数转换") {
        std::any result = string_to_any<CustomType>("42");
        CHECK(std::any_cast<CustomType>(result).getValue() == 42);
    }
    
    SUBCASE("负数") {
        std::any result = string_to_any<CustomType>("-100");
        CHECK(std::any_cast<CustomType>(result).getValue() == -100);
    }
}

TEST_CASE("往返转换 - any_to_string + string_to_any") {
    SUBCASE("int32_t 往返") {
        int32_t original = 12345;
        std::any data = original;
        std::string str = any_to_string<int32_t>(data);
        std::any result = string_to_any<int32_t>(str);
        CHECK(std::any_cast<int32_t>(result) == original);
    }
    
    SUBCASE("double 往返") {
        double original = 3.14159;
        std::any data = original;
        std::string str = any_to_string<double>(data);
        std::any result = string_to_any<double>(str);
        CHECK(std::abs(std::any_cast<double>(result) - original) < 0.0001);
    }
    
    SUBCASE("std::string 往返") {
        std::string original = "Hello Test";
        std::any result = string_to_any<std::string>(original);
        CHECK(std::any_cast<std::string>(result) == original);
    }
    
    SUBCASE("CustomType 往返") {
        CustomType original(999);
        std::any data = original;
        std::string str = any_to_string<CustomType>(data);
        // CustomType(999) -> 提取数字部分需要解析，这里简化测试
        CHECK(str == "CustomType(999)");
    }
}

TEST_CASE("边界值测试") {
    SUBCASE("int8_t 最大值") {
        std::any result = string_to_any<int8_t>("127");
        CHECK(std::any_cast<int8_t>(result) == 127);
    }
    
    SUBCASE("int8_t 最小值") {
        std::any result = string_to_any<int8_t>("-128");
        CHECK(std::any_cast<int8_t>(result) == -128);
    }
    
    SUBCASE("uint8_t 最大值") {
        std::any result = string_to_any<uint8_t>("255");
        CHECK(std::any_cast<uint8_t>(result) == 255);
    }
    
    SUBCASE("零值") {
        std::any result = string_to_any<int32_t>("0");
        CHECK(std::any_cast<int32_t>(result) == 0);
    }
    
    SUBCASE("浮点零值") {
        std::any result = string_to_any<double>("0.0");
        CHECK(std::any_cast<double>(result) == 0.0);
    }
}

