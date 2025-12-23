#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <concepts>
#include <ranges>
#include <cmath>
#include <limits>
#include <cstddef>
#include <cstdint>
#include <cctype>

namespace sequoia::utils {

std::string utf8_to_gbk(const std::string& str);
std::string gbk_to_utf8(const std::string& str);

/**
 * @brief 对数值进行四舍五入，支持四舍六入五成双规则
 * 
 * @tparam ValueType 数值类型（必须是浮点类型）
 * @param value 要四舍五入的数值
 * @param ndigits 保留小数位数
 * @return ValueType 四舍五入后的结果
 * 
 * @details 使用银行家舍入法（四舍六入五成双）
 */
template <std::floating_point ValueType>
[[nodiscard]] constexpr ValueType roundEx(ValueType value, int32_t ndigits = 0) noexcept {
    ValueType scale = 1;
    const int32_t ndigits_abs = std::abs(ndigits);
    
    for (int32_t i = 0; i < ndigits_abs; ++i) {
        scale *= 10;
    }

    const ValueType scaled_value = (ndigits >= 0) ? (value * scale) : (value / scale);
    
    // 四舍五入
    ValueType rounded_value = std::round(scaled_value);
    
    // 处理中间值情况 (0.5) - 银行家舍入
    ValueType diff = std::abs(rounded_value - scaled_value);
    if (std::abs(diff - static_cast<ValueType>(0.5)) < 
        std::numeric_limits<ValueType>::epsilon()) {
        rounded_value = static_cast<ValueType>(2) * std::round(scaled_value / static_cast<ValueType>(2));
    }

    return (ndigits >= 0) ? (rounded_value / scale) : (rounded_value * scale);
}

/**
 * @brief 对数值进行向上取整
 * 
 * @tparam ValueType 数值类型（必须是浮点类型）
 * @param value 要向上取整的数值
 * @param ndigits 保留小数位数
 * @return ValueType 向上取整后的结果
 * 
 * @details
 * 1. 正数向上取整 (正整数强制 + 1)
 * 2. 负数向下取整 (负整数强制 - 1)
 */
template <std::floating_point ValueType>
[[nodiscard]] constexpr ValueType ceilEx(ValueType value, int32_t ndigits = 0) noexcept {
    ValueType scale = 1;
    const int32_t ndigits_abs = std::abs(ndigits);
    
    for (int32_t i = 0; i < ndigits_abs; ++i) {
        scale *= 10;
    }

    ValueType scaled_value = (ndigits >= 0) ? (value * scale) : (value / scale);
    
    scaled_value = (scaled_value >= 0.0) 
        ? std::floor(scaled_value + 1.0)
        : std::ceil(scaled_value - 1.0);

    return (ndigits >= 0) ? (scaled_value / scale) : (scaled_value * scale);
}

/**
 * @brief 对数值进行向下取整
 * 
 * @tparam ValueType 数值类型（必须是浮点类型）
 * @param value 要向下取整的数值
 * @param ndigits 保留小数位数
 * @return ValueType 向下取整后的结果
 * 
 * @details
 * 1. 正数向下取整 (正整数不强制 - 1)
 * 2. 负数向上取整 (负整数不强制 + 1)
 */
template <std::floating_point ValueType>
[[nodiscard]] constexpr ValueType floorEx(ValueType value, int32_t ndigits = 0) noexcept {
    ValueType scale = 1;
    const int32_t ndigits_abs = std::abs(ndigits);
    
    for (int32_t i = 0; i < ndigits_abs; ++i) {
        scale *= 10;
    }

    ValueType scaled_value = (ndigits >= 0) ? (value * scale) : (value / scale);

    scaled_value = (scaled_value >= 0.0) 
        ? std::floor(scaled_value)
        : std::ceil(scaled_value);

    return (ndigits >= 0) ? (scaled_value / scale) : (scaled_value * scale);
}

/**
 * @brief 将字符串转换为小写（使用 C++20 ranges）
 * 
 * @param str 要转换的字符串
 */
inline void toLower(std::string& str) {
    std::ranges::transform(str, str.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
}

/**
 * @brief 将字符串转换为大写（使用 C++20 ranges）
 * 
 * @param str 要转换的字符串
 */
inline void toUpper(std::string& str) {
    std::ranges::transform(str, str.begin(), [](unsigned char c) {
        return std::toupper(c);
    });
}

/**
 * @brief 删除字符串两端的空格（使用 C++20 ranges）
 * 
 * @param str 要处理的字符串
 */
inline void trim(std::string& str) {
    if (str.empty()) {
        return;
    }
    
    constexpr std::string_view whitespace = " \t\n\r";
    
    auto first = std::ranges::find_if(str, [&](char c) {
        return whitespace.find(c) == std::string_view::npos;
    });
    
    if (first == str.end()) {
        str.clear();
        return;
    }
    
    auto last = std::ranges::find_if(str | std::views::reverse, [&](char c) {
        return whitespace.find(c) == std::string_view::npos;
    }).base();
    
    str = std::string(first, last);
}


/**
 * @brief 将字符串按指定分隔符分割为多个子字符串
 * 
 * @param str_view 要分割的字符串视图（支持 std::string 隐式转换）
 * @param delimiter 分隔符
 * @return std::vector<std::string_view> 分割后的子字符串
 * @note 注意返回结果的生命周期应与输入的字符串相同！
 */
[[nodiscard]] inline std::vector<std::string_view> split(std::string_view str_view, char delimiter) {
    std::vector<std::string_view> result;
    size_t prepos = 0;
    size_t pos = str_view.find_first_of(delimiter, prepos);
    
    while (pos != std::string_view::npos) {
        result.emplace_back(str_view.substr(prepos, pos - prepos));
        prepos = pos + 1;
        pos = str_view.find_first_of(delimiter, prepos);
    }
    
    result.emplace_back(str_view.substr(prepos));
    return result;
}

/**
 * @brief 将字符串按指定分隔符分割为多个子字符串
 * 
 * @param str_view 要分割的字符串视图
 * @param split_str 分隔符字符串
 * @return std::vector<std::string_view> 分割后的子字符串
 * @note 注意返回结果的生命周期应与输入的字符串相同！
 */
[[nodiscard]] inline std::vector<std::string_view> split(std::string_view str_view, std::string_view split_str) {
    std::vector<std::string_view> result;
    
    if (split_str.empty()) {
        result.emplace_back(str_view);
        return result;
    }
    
    size_t prepos = 0;
    size_t pos = str_view.find(split_str, prepos);
    
    while (pos != std::string_view::npos) {
        result.emplace_back(str_view.substr(prepos, pos - prepos));
        prepos = pos + split_str.size();
        pos = str_view.find(split_str, prepos);
    }
    
    result.emplace_back(str_view.substr(prepos));
    return result;
}

/**
 * @brief 将字节数组转换为十六进制字符串 如："abcd" -> "61626364"
 * 
 * @param bytes 字节数组
 * @param len 字节数组长度
 * @return std::string 十六进制字符串
 */
[[nodiscard]] inline std::string byteToHex(const char* bytes, size_t len) {
    if (bytes == nullptr) {
        return {};
    }
    
    std::string result;
    result.reserve(len * 2);
    
    const auto* p = reinterpret_cast<const unsigned char*>(bytes);
    for (size_t i = 0; i < len; ++i) {
        const unsigned char high = p[i] >> 4;
        const unsigned char low = p[i] & 0x0F;
        result.push_back(high < 10 ? '0' + high : 'A' + high - 10);
        result.push_back(low < 10 ? '0' + low : 'A' + low - 10);
    }
    
    return result;
}

[[nodiscard]] inline std::string byteToHex(std::string_view bytes) {
    return byteToHex(bytes.data(), bytes.size());
}

/**
 * @brief 将字节数组转换为十六进制字符串 如："abcd" -> "0x61 0x62 0x63 0x64"
 * 
 * @param bytes 字节数组
 * @param len 字节数组长度
 * @return std::string 十六进制字符串
 */
[[nodiscard]] inline std::string byteToHexWithPrefix(const char* bytes, size_t len) {
    if (bytes == nullptr) {
        return {};
    }
    
    std::string result;
    result.reserve(len * 5);
    
    const auto* p = reinterpret_cast<const unsigned char*>(bytes);
    for (size_t i = 0; i < len; ++i) {
        const unsigned char high = p[i] >> 4;
        const unsigned char low = p[i] & 0x0F;
        
        result.append("0x");
        result.push_back(high < 10 ? '0' + high : 'A' + high - 10);
        result.push_back(low < 10 ? '0' + low : 'A' + low - 10);
        result.push_back(' ');
    }
    
    return result;
}

[[nodiscard]] inline std::string byteToHexWithPrefix(std::string_view bytes) {
    return byteToHexWithPrefix(bytes.data(), bytes.size());
}

} // namespace sequoia::utils
