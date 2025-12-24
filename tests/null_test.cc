#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <sequoia/utils/null.h>
#include <limits>
#include <cmath>
#include <string>

using namespace sequoia::utils;

TEST_CASE("Null - 整数类型返回最大值") {
    SUBCASE("int32_t") {
        Null<int32_t> null_val;
        int32_t result = null_val;
        CHECK(result == std::numeric_limits<int32_t>::max());
    }
    
    SUBCASE("uint32_t") {
        Null<uint32_t> null_val;
        uint32_t result = null_val;
        CHECK(result == std::numeric_limits<uint32_t>::max());
    }
    
    SUBCASE("int64_t") {
        Null<int64_t> null_val;
        int64_t result = null_val;
        CHECK(result == std::numeric_limits<int64_t>::max());
    }
    
    SUBCASE("uint64_t") {
        Null<uint64_t> null_val;
        uint64_t result = null_val;
        CHECK(result == std::numeric_limits<uint64_t>::max());
    }
    
    SUBCASE("size_t") {
        Null<size_t> null_val;
        size_t result = null_val;
        CHECK(result == std::numeric_limits<size_t>::max());
    }
    
    SUBCASE("int16_t") {
        Null<int16_t> null_val;
        int16_t result = null_val;
        CHECK(result == std::numeric_limits<int16_t>::max());
    }
    
    SUBCASE("uint16_t") {
        Null<uint16_t> null_val;
        uint16_t result = null_val;
        CHECK(result == std::numeric_limits<uint16_t>::max());
    }
    
    SUBCASE("int8_t") {
        Null<int8_t> null_val;
        int8_t result = null_val;
        CHECK(result == std::numeric_limits<int8_t>::max());
    }
    
    SUBCASE("uint8_t") {
        Null<uint8_t> null_val;
        uint8_t result = null_val;
        CHECK(result == std::numeric_limits<uint8_t>::max());
    }
}

TEST_CASE("Null - 浮点类型返回 NaN") {
    SUBCASE("float") {
        Null<float> null_val;
        float result = null_val;
        CHECK(std::isnan(result));
    }
    
    SUBCASE("double") {
        Null<double> null_val;
        double result = null_val;
        CHECK(std::isnan(result));
    }
    
    SUBCASE("long double") {
        Null<long double> null_val;
        long double result = null_val;
        CHECK(std::isnan(result));
    }
}

TEST_CASE("Null - 其他类型返回默认值") {
    SUBCASE("std::string") {
        Null<std::string> null_val;
        std::string result = null_val;
        CHECK(result.empty());
        CHECK(result == "");
    }
    
    SUBCASE("bool") {
        Null<bool> null_val;
        bool result = null_val;
        // bool 是整数类型，返回最大值 true
        CHECK(result == true);
    }
}

TEST_CASE("Null - 浮点类型相等性比较") {
    SUBCASE("Null<float> 与 float NaN 比较") {
        Null<float> null_val;
        float nan_value = std::numeric_limits<float>::quiet_NaN();
        
        // 使用成员函数比较
        CHECK(null_val == nan_value);
        
        // 使用全局运算符比较
        CHECK(nan_value == null_val);
    }
    
    SUBCASE("Null<float> 与 double NaN 比较") {
        Null<float> null_val;
        double nan_value = std::numeric_limits<double>::quiet_NaN();
        
        CHECK(null_val == nan_value);
        CHECK(nan_value == null_val);
    }
    
    SUBCASE("Null<double> 与 float NaN 比较") {
        Null<double> null_val;
        float nan_value = std::numeric_limits<float>::quiet_NaN();
        
        CHECK(null_val == nan_value);
        CHECK(nan_value == null_val);
    }
    
    SUBCASE("Null<double> 与 double NaN 比较") {
        Null<double> null_val;
        double nan_value = std::numeric_limits<double>::quiet_NaN();
        
        CHECK(null_val == nan_value);
        CHECK(nan_value == null_val);
    }
    
    SUBCASE("与非 NaN 值比较应返回 false") {
        Null<float> null_val;
        
        CHECK_FALSE(null_val == 0.0f);
        CHECK_FALSE(null_val == 1.0f);
        CHECK_FALSE(null_val == -1.0f);
        CHECK_FALSE(null_val == 3.14f);
        
        CHECK_FALSE(0.0f == null_val);
        CHECK_FALSE(1.0f == null_val);
        CHECK_FALSE(-1.0f == null_val);
    }
}

TEST_CASE("Null - constexpr 支持") {
    SUBCASE("编译期计算 - 整数类型") {
        constexpr Null<int32_t> null_val;
        constexpr int32_t result = null_val;
        static_assert(result == std::numeric_limits<int32_t>::max());
        CHECK(result == std::numeric_limits<int32_t>::max());
    }
    
    SUBCASE("编译期计算 - uint64_t") {
        constexpr Null<uint64_t> null_val;
        constexpr uint64_t result = null_val;
        static_assert(result == std::numeric_limits<uint64_t>::max());
        CHECK(result == std::numeric_limits<uint64_t>::max());
    }
    
    SUBCASE("编译期计算 - 浮点类型") {
        constexpr Null<float> null_val;
        constexpr float result = null_val;
        // NaN 在编译期可以创建
        CHECK(std::isnan(result));
    }
    
    SUBCASE("编译期构造") {
        constexpr Null<int> null_int;
        constexpr Null<float> null_float;
        constexpr Null<double> null_double;
        
        // 验证可以在编译期构造
        CHECK(static_cast<int>(null_int) == std::numeric_limits<int>::max());
    }
}

TEST_CASE("Null - 隐式类型转换") {
    SUBCASE("赋值给变量") {
        Null<int32_t> null_val;
        int32_t x = null_val;
        CHECK(x == std::numeric_limits<int32_t>::max());
    }
    
    SUBCASE("作为函数参数") {
        auto check_value = [](int32_t val) {
            return val == std::numeric_limits<int32_t>::max();
        };
        
        Null<int32_t> null_val;
        CHECK(check_value(null_val));
    }
    
    SUBCASE("用于比较") {
        Null<int32_t> null_val;
        int32_t max_val = std::numeric_limits<int32_t>::max();
        
        CHECK(static_cast<int32_t>(null_val) == max_val);
    }
}

TEST_CASE("Null - 默认构造") {
    SUBCASE("int32_t 默认构造") {
        Null<int32_t> null_val1;
        Null<int32_t> null_val2{};
        
        CHECK(static_cast<int32_t>(null_val1) == std::numeric_limits<int32_t>::max());
        CHECK(static_cast<int32_t>(null_val2) == std::numeric_limits<int32_t>::max());
    }
    
    SUBCASE("float 默认构造") {
        Null<float> null_val1;
        Null<float> null_val2{};
        
        CHECK(std::isnan(static_cast<float>(null_val1)));
        CHECK(std::isnan(static_cast<float>(null_val2)));
    }
}

TEST_CASE("Null - 多次转换") {
    SUBCASE("多次隐式转换应保持一致") {
        Null<int32_t> null_val;
        
        int32_t result1 = null_val;
        int32_t result2 = null_val;
        int32_t result3 = null_val;
        
        CHECK(result1 == result2);
        CHECK(result2 == result3);
        CHECK(result1 == std::numeric_limits<int32_t>::max());
    }
    
    SUBCASE("浮点类型多次转换") {
        Null<float> null_val;
        
        float result1 = null_val;
        float result2 = null_val;
        
        CHECK(std::isnan(result1));
        CHECK(std::isnan(result2));
    }
}

TEST_CASE("Null - 不同大小的整数类型") {
    SUBCASE("验证不同整数类型返回各自的最大值") {
        CHECK(static_cast<int8_t>(Null<int8_t>{}) == 127);
        CHECK(static_cast<uint8_t>(Null<uint8_t>{}) == 255);
        CHECK(static_cast<int16_t>(Null<int16_t>{}) == 32767);
        CHECK(static_cast<uint16_t>(Null<uint16_t>{}) == 65535);
        CHECK(static_cast<int32_t>(Null<int32_t>{}) == 2147483647);
        CHECK(static_cast<uint32_t>(Null<uint32_t>{}) == 4294967295U);
    }
}

TEST_CASE("Null - C++20 Concepts 正确性") {
    SUBCASE("整数类型满足 Integral concept") {
        static_assert(Integral<int>);
        static_assert(Integral<int32_t>);
        static_assert(Integral<uint64_t>);
        static_assert(Integral<size_t>);
        static_assert(!Integral<float>);
        static_assert(!Integral<double>);
    }
    
    SUBCASE("浮点类型满足 FloatingPoint concept") {
        static_assert(FloatingPoint<float>);
        static_assert(FloatingPoint<double>);
        static_assert(FloatingPoint<long double>);
        static_assert(!FloatingPoint<int>);
        static_assert(!FloatingPoint<uint32_t>);
    }
}

TEST_CASE("Null - 特殊浮点值比较") {
    SUBCASE("signaling NaN") {
        Null<double> null_val;
        double signaling_nan = std::numeric_limits<double>::signaling_NaN();
        
        CHECK(null_val == signaling_nan);
        CHECK(signaling_nan == null_val);
    }
    
    SUBCASE("infinity 不应等于 Null") {
        Null<float> null_val;
        float inf = std::numeric_limits<float>::infinity();
        float neg_inf = -std::numeric_limits<float>::infinity();
        
        CHECK_FALSE(null_val == inf);
        CHECK_FALSE(null_val == neg_inf);
        CHECK_FALSE(inf == null_val);
        CHECK_FALSE(neg_inf == null_val);
    }
}

