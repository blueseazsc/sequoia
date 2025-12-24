#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <sequoia/utils/params.h>

using namespace sequoia::utils;

// ==================== 基本功能测试 ====================

TEST_CASE("Params - 基本构造和属性") {
    Params params;
    
    SUBCASE("初始状态为空") {
        CHECK(params.empty());
        CHECK(params.size() == 0);
    }
}

TEST_CASE("Params - set 和 get 基本操作") {
    Params params;
    
    SUBCASE("设置和获取 bool 类型") {
        params.set("flag", true);
        CHECK(params.have("flag"));
        CHECK(params.get<bool>("flag") == true);
        CHECK(params.size() == 1);
    }
    
    SUBCASE("设置和获取 int 类型") {
        params.set("count", 42);
        CHECK(params.have("count"));
        CHECK(params.get<int>("count") == 42);
    }
    
    SUBCASE("设置和获取 int64_t 类型") {
        params.set("bignum", 9223372036854775807LL);
        CHECK(params.have("bignum"));
        CHECK(params.get<int64_t>("bignum") == 9223372036854775807LL);
    }
    
    SUBCASE("设置和获取 double 类型") {
        params.set("pi", 3.14159);
        CHECK(params.have("pi"));
        CHECK(params.get<double>("pi") == doctest::Approx(3.14159));
    }
    
    SUBCASE("设置多个参数") {
        params.set("a", 1);
        params.set("b", 2.5);
        params.set("c", true);
        
        CHECK(params.size() == 3);
        CHECK(params.get<int>("a") == 1);
        CHECK(params.get<double>("b") == doctest::Approx(2.5));
        CHECK(params.get<bool>("c") == true);
    }
}

TEST_CASE("Params - 参数覆盖") {
    Params params;
    
    SUBCASE("相同类型覆盖") {
        params.set("value", 10);
        CHECK(params.get<int>("value") == 10);
        
        params.set("value", 20);
        CHECK(params.get<int>("value") == 20);
        CHECK(params.size() == 1);  // 大小不变
    }
    
    SUBCASE("int 和 int64_t 互相兼容") {
        params.set("num", 100);
        CHECK(params.get<int>("num") == 100);
        
        // 允许 int 和 int64_t 之间的转换
        params.set("num", 200LL);
        CHECK(params.get<int64_t>("num") == 200LL);
    }
    
    SUBCASE("不兼容类型抛出异常") {
        params.set("value", 42);
        CHECK_THROWS_AS(params.set("value", 3.14), std::logic_error);
    }
}

// ==================== 类型转换测试 ====================

TEST_CASE("Params - 整数类型自动转换") {
    Params params;
    
    SUBCASE("int 存储，int64_t 读取") {
        params.set("num", 123);
        CHECK(params.get<int64_t>("num") == 123LL);
    }
    
    SUBCASE("int64_t 存储，int 读取") {
        params.set("num", 456LL);
        CHECK(params.get<int>("num") == 456);
    }
    
    SUBCASE("超出 int 范围的 int64_t") {
        params.set("bignum", 9223372036854775807LL);
        // 这里会发生截断，但不会抛出异常
        auto result = params.get<int>("bignum");
        (void)result;  // 只是确保能够调用
    }
}

// ==================== try_get 测试 ====================

TEST_CASE("Params - try_get 默认值功能") {
    Params params;
    
    SUBCASE("参数不存在时返回默认值") {
        int result = params.try_get("missing", 99);
        CHECK(result == 99);
    }
    
    SUBCASE("参数存在时返回实际值") {
        params.set("value", 42);
        int result = params.try_get("value", 99);
        CHECK(result == 42);
    }
    
    SUBCASE("类型不匹配时返回默认值") {
        params.set("value", 42);
        double result = params.try_get<double>("value", 3.14);
        CHECK(result == doctest::Approx(3.14));
    }
    
    SUBCASE("不同类型的默认值") {
        CHECK(params.try_get("bool", true) == true);
        CHECK(params.try_get("int", 10) == 10);
        CHECK(params.try_get("int64", 100LL) == 100LL);
        CHECK(params.try_get("double", 2.5) == doctest::Approx(2.5));
    }
}

// ==================== 异常测试 ====================

TEST_CASE("Params - 异常处理") {
    Params params;
    
    SUBCASE("获取不存在的参数抛出异常") {
        CHECK_THROWS_AS([&]{ (void)params.get<int>("missing"); }(), std::out_of_range);
    }
    
    SUBCASE("类型不匹配抛出异常") {
        params.set("value", 42);
        CHECK_THROWS_AS([&]{ (void)params.get<double>("value"); }(), std::runtime_error);
    }
    
    SUBCASE("异常消息包含参数名") {
        try {
            (void)params.get<int>("test_key");
            FAIL("应该抛出异常");
        } catch (const std::out_of_range& e) {
            std::string msg(e.what());
            CHECK(msg.find("test_key") != std::string::npos);
        }
    }
}

// ==================== 辅助方法测试 ====================

TEST_CASE("Params - have 方法") {
    Params params;
    
    params.set("exists", 1);
    
    CHECK(params.have("exists") == true);
    CHECK(params.have("not_exists") == false);
    CHECK(params.have("") == false);
}

TEST_CASE("Params - type 方法") {
    Params params;
    
    SUBCASE("各种类型的名称") {
        params.set("b", true);
        params.set("i", 42);
        params.set("l", 100LL);
        params.set("d", 3.14);
        
        CHECK(params.type("b") == "bool");
        CHECK(params.type("i") == "int");
        CHECK(params.type("l") == "int64");
        CHECK(params.type("d") == "double");
    }
    
    SUBCASE("不存在的键抛出异常") {
        CHECK_THROWS_AS([&]{ (void)params.type("missing"); }(), std::out_of_range);
    }
}

TEST_CASE("Params - keys 方法") {
    Params params;
    
    SUBCASE("空参数集") {
        auto keys = params.keys();
        CHECK(keys.empty());
    }
    
    SUBCASE("多个参数") {
        params.set("a", 1);
        params.set("b", 2);
        params.set("c", 3);
        
        auto keys = params.keys();
        CHECK(keys.size() == 3);
        
        // map 按字母顺序存储
        CHECK(keys[0] == "a");
        CHECK(keys[1] == "b");
        CHECK(keys[2] == "c");
    }
}

TEST_CASE("Params - to_string 方法") {
    Params params;
    
    SUBCASE("空参数集") {
        std::string str = params.to_string();
        CHECK(str.empty());
    }
    
    SUBCASE("包含多个参数") {
        params.set("flag", true);
        params.set("count", 10);
        params.set("value", 3.14);
        
        std::string str = params.to_string();
        
        // 检查是否包含所有键
        CHECK(str.find("flag") != std::string::npos);
        CHECK(str.find("count") != std::string::npos);
        CHECK(str.find("value") != std::string::npos);
        
        // 检查是否包含值
        CHECK(str.find("flag=true") != std::string::npos);
        CHECK(str.find("count=10") != std::string::npos);
        CHECK(str.find("3.14") != std::string::npos);
    }
    
    SUBCASE("布尔值 true/false 格式") {
        params.set("bool_true", true);
        params.set("bool_false", false);
        
        std::string str = params.to_string();
        
        // 检查布尔值显示为 true/false 而不是 1/0
        CHECK(str.find("bool_true=true") != std::string::npos);
        CHECK(str.find("bool_false=false") != std::string::npos);
    }
}

TEST_CASE("Params - remove 方法") {
    Params params;
    
    SUBCASE("删除存在的参数") {
        params.set("key", 42);
        CHECK(params.have("key"));
        
        bool removed = params.remove("key");
        CHECK(removed == true);
        CHECK(params.have("key") == false);
        CHECK(params.empty());
    }
    
    SUBCASE("删除不存在的参数") {
        bool removed = params.remove("not_exists");
        CHECK(removed == false);
    }
}

TEST_CASE("Params - clear 方法") {
    Params params;
    
    params.set("a", 1);
    params.set("b", 2);
    params.set("c", 3);
    
    CHECK(params.size() == 3);
    
    params.clear();
    
    CHECK(params.empty());
    CHECK(params.size() == 0);
}

// ==================== 比较运算符测试 ====================

TEST_CASE("Params - operator==") {
    SUBCASE("两个空参数集相等") {
        Params p1, p2;
        CHECK(p1 == p2);
    }
    
    SUBCASE("相同内容的参数集相等") {
        Params p1, p2;
        p1.set("a", 1);
        p1.set("b", 2.5);
        
        p2.set("a", 1);
        p2.set("b", 2.5);
        
        CHECK(p1 == p2);
    }
    
    SUBCASE("不同内容的参数集不相等") {
        Params p1, p2;
        p1.set("a", 1);
        p2.set("a", 2);
        
        CHECK(!(p1 == p2));
    }
    
    SUBCASE("键不同的参数集不相等") {
        Params p1, p2;
        p1.set("a", 1);
        p2.set("b", 1);
        
        CHECK(!(p1 == p2));
    }
    
    SUBCASE("大小不同的参数集不相等") {
        Params p1, p2;
        p1.set("a", 1);
        p2.set("a", 1);
        p2.set("b", 2);
        
        CHECK(!(p1 == p2));
    }
}

TEST_CASE("Params - operator<=> (三路比较)") {
    SUBCASE("大小不同的比较") {
        Params p1, p2;
        p1.set("a", 1);
        
        p2.set("a", 1);
        p2.set("b", 2);
        
        CHECK(p1 < p2);
        CHECK(p2 > p1);
    }
    
    SUBCASE("相同大小不同键的比较") {
        Params p1, p2;
        p1.set("a", 1);
        p2.set("b", 1);
        
        CHECK(p1 < p2);  // "a" < "b"
    }
    
    SUBCASE("相同键不同值的比较") {
        Params p1, p2;
        p1.set("key", 1);
        p2.set("key", 2);
        
        CHECK(p1 < p2);
    }
}

// ==================== 输出运算符测试 ====================

TEST_CASE("Params - operator<<") {
    Params params;
    params.set("name", 42);
    params.set("flag", true);
    
    std::ostringstream oss;
    oss << params;
    
    std::string output = oss.str();
    
    CHECK(output.find("Params[") != std::string::npos);
    CHECK(output.find("name") != std::string::npos);
    CHECK(output.find("flag") != std::string::npos);
}

// ==================== 迭代器测试 ====================

TEST_CASE("Params - 迭代器") {
    Params params;
    params.set("a", 1);
    params.set("b", 2);
    params.set("c", 3);
    
    SUBCASE("使用迭代器遍历") {
        int count = 0;
        for (auto it = params.begin(); it != params.end(); ++it) {
            CHECK(!it->first.empty());
            count++;
        }
        CHECK(count == 3);
    }
    
    SUBCASE("范围 for 循环") {
        int count = 0;
        for (const auto& [key, value] : params) {
            CHECK(!key.empty());
            count++;
        }
        CHECK(count == 3);
    }
}

// ==================== C++20 Ranges 测试 ====================

TEST_CASE("Params - keys_view (C++20 ranges)") {
    Params params;
    params.set("apple", 1);
    params.set("banana", 2);
    params.set("cherry", 3);
    
    SUBCASE("使用 keys_view 获取键") {
        std::vector<std::string> keys;
        for (const auto& key : params.keys_view()) {
            keys.push_back(key);
        }
        
        CHECK(keys.size() == 3);
        CHECK(keys[0] == "apple");
        CHECK(keys[1] == "banana");
        CHECK(keys[2] == "cherry");
    }
}

// ==================== support 静态方法测试 ====================

TEST_CASE("Params - support 静态方法") {
    SUBCASE("支持的类型") {
        CHECK(Params::support(std::any(true)) == true);
        CHECK(Params::support(std::any(42)) == true);
        CHECK(Params::support(std::any(100LL)) == true);
        CHECK(Params::support(std::any(3.14)) == true);
    }
    
    SUBCASE("不支持的类型") {
        CHECK(Params::support(std::any(std::string("test"))) == false);
        CHECK(Params::support(std::any()) == false);
    }
}

// ==================== 宏功能测试 ====================

class TestClassWithParams {
    PARAMETERS_SUPPORT
};

TEST_CASE("Params - PARAMETERS_SUPPORT 宏") {
    TestClassWithParams obj;
    
    SUBCASE("GetParams 和 SetParams") {
        Params params;
        params.set("test", 123);
        
        obj.SetParams(params);
        const auto& retrieved = obj.GetParams();
        
        CHECK(retrieved.get<int>("test") == 123);
    }
    
    SUBCASE("HaveParam") {
        obj.SetParam("key", 42);
        CHECK(obj.HaveParam("key") == true);
        CHECK(obj.HaveParam("missing") == false);
    }
    
    SUBCASE("GetParam") {
        obj.SetParam("value", 99);
        CHECK(obj.GetParam<int>("value") == 99);
    }
    
    SUBCASE("TryGetParam") {
        int result = obj.TryGetParam("missing", 100);
        CHECK(result == 100);
        
        obj.SetParam("exists", 50);
        result = obj.TryGetParam("exists", 100);
        CHECK(result == 50);
    }
    
    SUBCASE("SetParam") {
        obj.SetParam("a", 1);
        obj.SetParam("b", 2.5);
        obj.SetParam("c", true);
        
        CHECK(obj.GetParam<int>("a") == 1);
        CHECK(obj.GetParam<double>("b") == doctest::Approx(2.5));
        CHECK(obj.GetParam<bool>("c") == true);
    }
    
    SUBCASE("GetParamFromOther") {
        Params other;
        other.set("shared", 777);
        
        int value = obj.GetParamFromOther(other, "shared", 0);
        CHECK(value == 777);
        CHECK(obj.HaveParam("shared"));
    }
    
    SUBCASE("GetParamFromOther 使用默认值") {
        Params other;
        
        int value = obj.GetParamFromOther(other, "missing", 888);
        CHECK(value == 888);
        CHECK(obj.HaveParam("missing"));
        CHECK(obj.GetParam<int>("missing") == 888);
    }
    
    SUBCASE("RemoveParam") {
        obj.SetParam("temp", 123);
        CHECK(obj.HaveParam("temp"));
        
        bool removed = obj.RemoveParam("temp");
        CHECK(removed == true);
        CHECK(obj.HaveParam("temp") == false);
    }
    
    SUBCASE("ClearParams") {
        obj.SetParam("a", 1);
        obj.SetParam("b", 2);
        
        obj.ClearParams();
        CHECK(obj.GetParams().empty());
    }
}

// ==================== 边界条件测试 ====================

TEST_CASE("Params - 边界条件") {
    Params params;
    
    SUBCASE("空键名") {
        params.set("", 42);
        CHECK(params.have(""));
        CHECK(params.get<int>("") == 42);
    }
    
    SUBCASE("很长的键名") {
        std::string long_key(1000, 'a');
        params.set(long_key, 999);
        CHECK(params.have(long_key));
        CHECK(params.get<int>(long_key) == 999);
    }
    
    SUBCASE("极值测试") {
        params.set("max_int", std::numeric_limits<int>::max());
        params.set("min_int", std::numeric_limits<int>::min());
        params.set("max_int64", std::numeric_limits<int64_t>::max());
        params.set("min_int64", std::numeric_limits<int64_t>::min());
        params.set("max_double", std::numeric_limits<double>::max());
        params.set("min_double", std::numeric_limits<double>::lowest());
        
        CHECK(params.get<int>("max_int") == std::numeric_limits<int>::max());
        CHECK(params.get<int>("min_int") == std::numeric_limits<int>::min());
        CHECK(params.get<int64_t>("max_int64") == std::numeric_limits<int64_t>::max());
        CHECK(params.get<int64_t>("min_int64") == std::numeric_limits<int64_t>::min());
        CHECK(params.get<double>("max_double") == std::numeric_limits<double>::max());
        CHECK(params.get<double>("min_double") == std::numeric_limits<double>::lowest());
    }
    
    SUBCASE("零值测试") {
        params.set("zero_int", 0);
        params.set("zero_double", 0.0);
        params.set("false_bool", false);
        
        CHECK(params.get<int>("zero_int") == 0);
        CHECK(params.get<double>("zero_double") == 0.0);
        CHECK(params.get<bool>("false_bool") == false);
    }
}

// ==================== 性能和容量测试 ====================

TEST_CASE("Params - 大量参数") {
    Params params;
    
    SUBCASE("添加大量参数") {
        const int count = 1000;
        for (int i = 0; i < count; ++i) {
            params.set("key_" + std::to_string(i), i);
        }
        
        CHECK(params.size() == count);
        
        // 验证几个随机值
        CHECK(params.get<int>("key_0") == 0);
        CHECK(params.get<int>("key_500") == 500);
        CHECK(params.get<int>("key_999") == 999);
    }
}

// ==================== 复制和移动测试 ====================

TEST_CASE("Params - 复制构造和赋值") {
    Params original;
    original.set("a", 1);
    original.set("b", 2.5);
    original.set("c", true);
    
    SUBCASE("复制构造") {
        Params copy(original);
        
        CHECK(copy.size() == original.size());
        CHECK(copy == original);
        CHECK(copy.get<int>("a") == 1);
        
        // 修改副本不影响原始
        copy.set("a", 999);
        CHECK(original.get<int>("a") == 1);
        CHECK(copy.get<int>("a") == 999);
    }
    
    SUBCASE("赋值运算符") {
        Params copy;
        copy = original;
        
        CHECK(copy.size() == original.size());
        CHECK(copy == original);
    }
}

