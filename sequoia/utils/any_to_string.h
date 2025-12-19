#pragma once

#include <any>
#include <string>
#include <type_traits>

namespace sequoia::utils {

/**
 * 针对数值类型的转换：使用 std::to_string
 */
template <typename ValueType>
std::enable_if_t<std::is_arithmetic_v<ValueType>, std::string>
any_to_string(const std::any& data) {
    return std::to_string(std::any_cast<ValueType>(data));
}

/**
 * 针对其他类型的转换：调用对象的 to_string() 方法
 */
template <typename ValueType>
std::enable_if_t<!std::is_arithmetic_v<ValueType>, std::string>
any_to_string(const std::any& data) {
    return std::any_cast<ValueType>(data).to_string();
}
/**
 * 针对数值类型的转换：使用 std::stoX
 */
template <typename ValueType>
std::enable_if_t<std::is_integral_v<ValueType>, std::any>
string_to_any(const std::string& str) {
    if constexpr (std::is_same_v<ValueType, bool>) {
        return std::any(str == "true" || str == "1");
    } else if constexpr (std::is_signed_v<ValueType>) {
        return std::any(static_cast<ValueType>(std::stoll(str)));
    } else {
        return std::any(static_cast<ValueType>(std::stoull(str)));
    }
}

template <typename ValueType>
std::enable_if_t<std::is_floating_point_v<ValueType>, std::any>
string_to_any(const std::string& str) {
    if constexpr (std::is_same_v<ValueType, float>) {
        return std::any(std::stof(str));
    } else if constexpr (std::is_same_v<ValueType, double>) {
        return std::any(std::stod(str));
    } else {
        return std::any(std::stold(str));
    }
}

/**
 * 针对 std::string
 */
template <typename ValueType>
std::enable_if_t<std::is_same_v<ValueType, std::string>, std::any>
string_to_any(const std::string& str) {
    return std::any(str);
}

/**
 * 针对其它类型：假设具有 static from_string(const std::string&) 方法
 */
template <typename ValueType>
std::enable_if_t<!std::is_arithmetic_v<ValueType> && !std::is_same_v<ValueType, std::string>, std::any>
string_to_any(const std::string& str) {
    return std::any(ValueType(str));
}

}  // namespace sequoia::utils