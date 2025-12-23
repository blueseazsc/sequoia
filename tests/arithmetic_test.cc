#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <sequoia/utils/arithmetic.h>
#include <cmath>
#include <limits>

using namespace sequoia::utils;

// ==================== roundEx 测试 ====================

TEST_CASE("roundEx - double 基本功能") {
    SUBCASE("正数四舍五入到整数") {
        CHECK(roundEx(3.4) == 3.0);
        CHECK(roundEx(3.5) == 4.0);
        CHECK(roundEx(3.6) == 4.0);
    }
    
    SUBCASE("负数四舍五入到整数") {
        CHECK(roundEx(-3.4) == -3.0);
        CHECK(roundEx(-3.5) == -4.0);
        CHECK(roundEx(-3.6) == -4.0);
    }
    
    SUBCASE("保留一位小数") {
        CHECK(std::abs(roundEx(3.14, 1) - 3.1) < 1e-10);
        CHECK(std::abs(roundEx(3.15, 1) - 3.2) < 1e-10);
        CHECK(std::abs(roundEx(3.16, 1) - 3.2) < 1e-10);
    }
    
    SUBCASE("保留两位小数") {
        CHECK(std::abs(roundEx(3.141, 2) - 3.14) < 1e-10);
        CHECK(std::abs(roundEx(3.145, 2) - 3.14) < 1e-10);
        CHECK(std::abs(roundEx(3.146, 2) - 3.15) < 1e-10);
    }
    
    SUBCASE("负小数位数（十位、百位）") {
        CHECK(roundEx(123.456, -1) == 120.0);  // 个位3，舍去
        CHECK(roundEx(125.456, -1) == 130.0);  // 个位5，进位
        CHECK(roundEx(126.456, -1) == 130.0);  // 个位6，进位
        CHECK(roundEx(124.456, -1) == 120.0);  // 个位4，舍去
        CHECK(roundEx(1234.567, -2) == 1200.0); // 十位3，舍去
        CHECK(roundEx(1254.567, -2) == 1300.0); // 十位5，进位
    }
    
    SUBCASE("银行家舍入法（四舍六入五成双）") {
        // x.5 时向最近的偶数舍入
        CHECK(roundEx(2.5) == 2.0);   // 向下到偶数 2
        CHECK(roundEx(3.5) == 4.0);   // 向上到偶数 4
        CHECK(roundEx(4.5) == 4.0);   // 向下到偶数 4
        CHECK(roundEx(5.5) == 6.0);   // 向上到偶数 6
        
        // 负数的情况
        CHECK(roundEx(-2.5) == -2.0); // 向上到偶数 -2
        CHECK(roundEx(-3.5) == -4.0); // 向下到偶数 -4
        
        // 保留小数位数时的银行家舍入
        CHECK(std::abs(roundEx(2.25, 1) - 2.2) < 1e-10);  // 向下到偶数
        CHECK(std::abs(roundEx(2.35, 1) - 2.4) < 1e-10);  // 向上到偶数
        CHECK(std::abs(roundEx(3.145, 2) - 3.14) < 1e-10); // 向下到偶数
        CHECK(std::abs(roundEx(3.155, 2) - 3.16) < 1e-10); // 向上到偶数
    }
    
    SUBCASE("零值") {
        CHECK(roundEx(0.0) == 0.0);
        CHECK(roundEx(0.0, 2) == 0.0);
    }
}

TEST_CASE("roundEx - float 基本功能") {
    SUBCASE("正数四舍五入") {
        CHECK(roundEx(3.4f) == 3.0f);
        CHECK(roundEx(3.5f) == 4.0f);
        CHECK(roundEx(3.6f) == 4.0f);
    }
    
    SUBCASE("保留一位小数") {
        CHECK(std::abs(roundEx(3.14f, 1) - 3.1f) < 1e-5f);
        CHECK(std::abs(roundEx(3.15f, 1) - 3.2f) < 1e-5f);
    }
}

// ==================== ceilEx 测试 ====================

TEST_CASE("ceilEx - double 基本功能") {
    SUBCASE("正数向上取整") {
        CHECK(ceilEx(3.1) == 4.0);
        CHECK(ceilEx(3.5) == 4.0);
        CHECK(ceilEx(3.9) == 4.0);
        CHECK(ceilEx(3.0) == 4.0);  // 正整数强制 +1
    }
    
    SUBCASE("负数向下取整") {
        CHECK(ceilEx(-3.1) == -4.0);
        CHECK(ceilEx(-3.5) == -4.0);
        CHECK(ceilEx(-3.9) == -4.0);
        CHECK(ceilEx(-3.0) == -4.0);  // 负整数强制 -1
    }
    
    SUBCASE("保留一位小数") {
        CHECK(std::abs(ceilEx(3.01, 1) - 3.1) < 1e-10);
        CHECK(std::abs(ceilEx(3.11, 1) - 3.2) < 1e-10);
        CHECK(std::abs(ceilEx(3.10, 1) - 3.2) < 1e-10);
    }
    
    SUBCASE("保留两位小数") {
        CHECK(std::abs(ceilEx(3.141, 2) - 3.15) < 1e-10);
        CHECK(std::abs(ceilEx(3.100, 2) - 3.11) < 1e-10);
    }
    
    SUBCASE("零值") {
        CHECK(ceilEx(0.0) == 1.0);  // 零是正数，向上取整后为 1
    }
}

TEST_CASE("ceilEx - float 基本功能") {
    SUBCASE("正数向上取整") {
        CHECK(ceilEx(3.1f) == 4.0f);
        CHECK(ceilEx(3.0f) == 4.0f);
    }
    
    SUBCASE("负数向下取整") {
        CHECK(ceilEx(-3.1f) == -4.0f);
        CHECK(ceilEx(-3.0f) == -4.0f);
    }
}

// ==================== floorEx 测试 ====================

TEST_CASE("floorEx - double 基本功能") {
    SUBCASE("正数向下取整") {
        CHECK(floorEx(3.1) == 3.0);
        CHECK(floorEx(3.5) == 3.0);
        CHECK(floorEx(3.9) == 3.0);
        CHECK(floorEx(3.0) == 3.0);  // 正整数不强制 -1
    }
    
    SUBCASE("负数向上取整") {
        CHECK(floorEx(-3.1) == -3.0);
        CHECK(floorEx(-3.5) == -3.0);
        CHECK(floorEx(-3.9) == -3.0);
        CHECK(floorEx(-3.0) == -3.0);  // 负整数不强制 +1
    }
    
    SUBCASE("保留一位小数") {
        CHECK(std::abs(floorEx(3.19, 1) - 3.1) < 1e-10);
        CHECK(std::abs(floorEx(3.15, 1) - 3.1) < 1e-10);
        CHECK(std::abs(floorEx(3.10, 1) - 3.1) < 1e-10);
    }
    
    SUBCASE("保留两位小数") {
        CHECK(std::abs(floorEx(3.149, 2) - 3.14) < 1e-10);
        CHECK(std::abs(floorEx(3.140, 2) - 3.14) < 1e-10);
    }
    
    SUBCASE("零值") {
        CHECK(floorEx(0.0) == 0.0);  // 零向下取整还是零
    }
}

TEST_CASE("floorEx - float 基本功能") {
    SUBCASE("正数向下取整") {
        CHECK(floorEx(3.9f) == 3.0f);
        CHECK(floorEx(3.0f) == 3.0f);
    }
    
    SUBCASE("负数向上取整") {
        CHECK(floorEx(-3.9f) == -3.0f);
        CHECK(floorEx(-3.0f) == -3.0f);
    }
}

// ==================== 边界值测试 ====================

TEST_CASE("算术函数 - 边界值测试") {
    SUBCASE("极小值") {
        double tiny = 1e-10;
        CHECK(std::abs(roundEx(tiny, 10)) < 1e-9);
        CHECK(floorEx(tiny) == 0.0);
        CHECK(ceilEx(tiny) == 1.0);
    }
    
    SUBCASE("极大值") {
        double large = 1e10;
        CHECK(roundEx(large) == large);
        CHECK(floorEx(large) == large);
        CHECK(ceilEx(large) == large + 1.0);
    }
    
    SUBCASE("负零") {
        CHECK(roundEx(-0.0) == 0.0);
        CHECK(floorEx(-0.0) == 0.0);
    }
}

// ==================== 字符编码转换测试 ====================

TEST_CASE("utf8_to_gbk - 基本功能") {
    SUBCASE("空字符串") {
        std::string empty = "";
        CHECK(utf8_to_gbk(empty) == "");
    }
    
    SUBCASE("纯英文字符串") {
        std::string english = "Hello World";
        std::string result = utf8_to_gbk(english);
        CHECK(result == "Hello World");
    }
    
    SUBCASE("中文字符串") {
        std::string chinese = "你好世界";
        std::string result = utf8_to_gbk(chinese);
        // GBK 编码后应该不等于原字符串（除非原本就是 GBK）
        CHECK(!result.empty());
        // 验证往返转换
        CHECK(gbk_to_utf8(result) == chinese);
    }
    
    SUBCASE("中英文混合") {
        std::string mixed = "Hello 你好 World";
        std::string gbk = utf8_to_gbk(mixed);
        CHECK(!gbk.empty());
        // 验证往返转换
        CHECK(gbk_to_utf8(gbk) == mixed);
    }
    
    SUBCASE("特殊符号") {
        std::string symbols = "123!@#$%^&*()";
        CHECK(utf8_to_gbk(symbols) == symbols);
    }
}

TEST_CASE("gbk_to_utf8 - 基本功能") {
    SUBCASE("空字符串") {
        std::string empty = "";
        CHECK(gbk_to_utf8(empty) == "");
    }
    
    SUBCASE("纯英文字符串") {
        std::string english = "Hello World";
        std::string result = gbk_to_utf8(english);
        CHECK(result == "Hello World");
    }
}

TEST_CASE("编码转换 - 往返测试") {
    SUBCASE("UTF-8 -> GBK -> UTF-8") {
        std::string original = "测试中文编码转换";
        std::string gbk = utf8_to_gbk(original);
        std::string back = gbk_to_utf8(gbk);
        CHECK(back == original);
    }
    
    SUBCASE("复杂中文文本") {
        std::string original = "这是一个用于测试字符编码转换功能的复杂文本，包含标点符号：，。！？";
        std::string gbk = utf8_to_gbk(original);
        std::string back = gbk_to_utf8(gbk);
        CHECK(back == original);
    }
    
    SUBCASE("数字和中文混合") {
        std::string original = "订单号：12345，金额：￥99.99";
        std::string gbk = utf8_to_gbk(original);
        std::string back = gbk_to_utf8(gbk);
        CHECK(back == original);
    }
}

// ==================== 字符串工具函数测试 ====================

TEST_CASE("toLower - 转换为小写") {
    SUBCASE("全大写") {
        std::string str = "HELLO WORLD";
        toLower(str);
        CHECK(str == "hello world");
    }
    
    SUBCASE("混合大小写") {
        std::string str = "Hello World";
        toLower(str);
        CHECK(str == "hello world");
    }
    
    SUBCASE("已经是小写") {
        std::string str = "hello world";
        toLower(str);
        CHECK(str == "hello world");
    }
    
    SUBCASE("包含数字和符号") {
        std::string str = "Hello123!@#";
        toLower(str);
        CHECK(str == "hello123!@#");
    }
}

TEST_CASE("toUpper - 转换为大写") {
    SUBCASE("全小写") {
        std::string str = "hello world";
        toUpper(str);
        CHECK(str == "HELLO WORLD");
    }
    
    SUBCASE("混合大小写") {
        std::string str = "Hello World";
        toUpper(str);
        CHECK(str == "HELLO WORLD");
    }
    
    SUBCASE("已经是大写") {
        std::string str = "HELLO WORLD";
        toUpper(str);
        CHECK(str == "HELLO WORLD");
    }
}

TEST_CASE("trim - 去除首尾空格") {
    SUBCASE("前后都有空格") {
        std::string str = "  hello world  ";
        trim(str);
        CHECK(str == "hello world");
    }
    
    SUBCASE("只有前面有空格") {
        std::string str = "  hello world";
        trim(str);
        CHECK(str == "hello world");
    }
    
    SUBCASE("只有后面有空格") {
        std::string str = "hello world  ";
        trim(str);
        CHECK(str == "hello world");
    }
    
    SUBCASE("中间的空格不移除") {
        std::string str = "  hello  world  ";
        trim(str);
        CHECK(str == "hello  world");
    }
    
    SUBCASE("包含制表符和换行符") {
        std::string str = "\t\n  hello world  \n\t";
        trim(str);
        CHECK(str == "hello world");
    }
    
    SUBCASE("全是空格") {
        std::string str = "    ";
        trim(str);
        CHECK(str.empty());
    }
    
    SUBCASE("空字符串") {
        std::string str = "";
        trim(str);
        CHECK(str.empty());
    }
}

TEST_CASE("split - 字符分割") {
    SUBCASE("使用逗号分割") {
        std::string str = "a,b,c,d";
        auto result = split(str, ',');
        CHECK(result.size() == 4);
        CHECK(result[0] == "a");
        CHECK(result[1] == "b");
        CHECK(result[2] == "c");
        CHECK(result[3] == "d");
    }
    
    SUBCASE("分割包含空格的字符串") {
        std::string str = "hello world test";
        auto result = split(str, ' ');
        CHECK(result.size() == 3);
        CHECK(result[0] == "hello");
        CHECK(result[1] == "world");
        CHECK(result[2] == "test");
    }
    
    SUBCASE("连续分隔符") {
        std::string str = "a,,b";
        auto result = split(str, ',');
        CHECK(result.size() == 3);
        CHECK(result[0] == "a");
        CHECK(result[1] == "");
        CHECK(result[2] == "b");
    }
    
    SUBCASE("没有分隔符") {
        std::string str = "hello";
        auto result = split(str, ',');
        CHECK(result.size() == 1);
        CHECK(result[0] == "hello");
    }
}

TEST_CASE("split - 字符串分割") {
    SUBCASE("使用多字符分隔符") {
        std::string str = "hello::world::test";
        auto result = split(str, "::");
        CHECK(result.size() == 3);
        CHECK(result[0] == "hello");
        CHECK(result[1] == "world");
        CHECK(result[2] == "test");
    }
    
    SUBCASE("空分隔符") {
        std::string str = "hello";
        auto result = split(str, "");
        CHECK(result.size() == 1);
        CHECK(result[0] == "hello");
    }
}

TEST_CASE("byteToHex - 字节转十六进制") {
    SUBCASE("简单字符串") {
        std::string input = "abc";
        std::string result = byteToHex(input);
        CHECK(result == "616263");  // 'a'=0x61, 'b'=0x62, 'c'=0x63
    }
    
    SUBCASE("空字符串") {
        std::string input = "";
        std::string result = byteToHex(input);
        CHECK(result.empty());
    }
    
    SUBCASE("使用指针和长度") {
        const char* data = "test";
        std::string result = byteToHex(data, 4);
        CHECK(result == "74657374");
    }
    
    SUBCASE("空指针") {
        std::string result = byteToHex(nullptr, 10);
        CHECK(result.empty());
    }
}

TEST_CASE("byteToHexWithPrefix - 字节转十六进制（带前缀）") {
    SUBCASE("简单字符串") {
        std::string input = "ab";
        std::string result = byteToHexWithPrefix(input);
        CHECK(result == "0x61 0x62 ");
    }
    
    SUBCASE("空字符串") {
        std::string input = "";
        std::string result = byteToHexWithPrefix(input);
        CHECK(result.empty());
    }
}

