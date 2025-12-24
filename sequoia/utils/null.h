#pragma once

#include <concepts>
#include <limits>
#include <cmath>

namespace sequoia::utils {

// C++20 concepts 定义类型约束
template<typename T>
concept Integral = std::is_integral_v<T>;

template<typename T>
concept FloatingPoint = std::floating_point<T>;

// 通用模板类 - 使用 C++20 特性优化
template<typename T>
class Null {
public:
    constexpr Null() = default;
    
    // 整数类型：返回最大值
    [[nodiscard]] constexpr operator T() const noexcept requires Integral<T> {
        return std::numeric_limits<T>::max();
    }
    
    // 浮点类型：返回 NaN
    [[nodiscard]] constexpr operator T() const noexcept requires FloatingPoint<T> {
        return std::numeric_limits<T>::quiet_NaN();
    }
    
    // 其他类型：返回默认构造值
    [[nodiscard]] constexpr operator T() const 
        requires (!Integral<T> && !FloatingPoint<T>) {
        return T();
    }
    
    // 浮点类型的相等性比较
    template<FloatingPoint U>
    [[nodiscard]] constexpr bool operator==(U value) const noexcept 
        requires FloatingPoint<T> {
        return std::isnan(value);
    }
};

// 浮点类型的全局相等性比较运算符
template<FloatingPoint T, FloatingPoint U>
[[nodiscard]] constexpr bool operator==(T value, const Null<U>&) noexcept {
    return std::isnan(value);
}

} // namespace sequoia::utils