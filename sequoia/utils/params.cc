#include "params.h"
#include <sstream>

namespace sequoia::utils {

namespace detail {

// C++20: 使用枚举类加强类型安全
enum class ParamType : uint8_t {
    Unknown,
    Bool,
    Int,
    Int64,
    Double,
};

// C++20: 使用结构化绑定和现代初始化
struct TypeHandler {
    std::string_view name;
    void (*output)(std::ostream&, const std::any&);
    void (*to_string)(std::ostream&, std::string_view, const std::any&);
    bool (*equal)(const std::any&, const std::any&);
    std::weak_ordering (*compare)(const std::any&, const std::any&);
};

// C++20: constexpr 字符串视图
constexpr std::string_view kSplitStr = ", ";

// C++20: 类型处理器的辅助函数
template<typename T>
inline void output_value(std::ostream& os, const std::any& value, std::string_view type_name) {
    if constexpr (std::is_same_v<T, bool>) {
        os << "(" << type_name << "): " << std::boolalpha << std::any_cast<T>(value) << kSplitStr;
    } else {
        os << "(" << type_name << "): " << std::any_cast<T>(value) << kSplitStr;
    }
}

template<typename T>
inline void to_string_value(std::ostream& os, std::string_view key, const std::any& value) {
    if constexpr (std::is_same_v<T, bool>) {
        os << key << "=" << std::boolalpha << std::any_cast<T>(value) << kSplitStr;
    } else {
        os << key << "=" << std::any_cast<T>(value) << kSplitStr;
    }
}

template<typename T>
inline bool equal_value(const std::any& a, const std::any& b) {
    return std::any_cast<T>(a) == std::any_cast<T>(b);
}

template<typename T>
inline std::weak_ordering compare_value(const std::any& a, const std::any& b) {
    auto val_a = std::any_cast<T>(a);
    auto val_b = std::any_cast<T>(b);
    
    // 对于浮点数，使用标准比较
    if constexpr (std::is_floating_point_v<T>) {
        if (val_a < val_b) return std::weak_ordering::less;
        if (val_a > val_b) return std::weak_ordering::greater;
        return std::weak_ordering::equivalent;
    } else {
        // 对于整数和布尔值，可以直接使用 <=>
        auto cmp = val_a <=> val_b;
        if (cmp < 0) return std::weak_ordering::less;
        if (cmp > 0) return std::weak_ordering::greater;
        return std::weak_ordering::equivalent;
    }
}

// C++20: 使用指针简化
inline const TypeHandler kBoolHandler = {
    .name = "bool",
    .output = [](std::ostream& os, const std::any& v) { output_value<bool>(os, v, "bool"); },
    .to_string = [](std::ostream& os, std::string_view k, const std::any& v) { to_string_value<bool>(os, k, v); },
    .equal = [](const std::any& a, const std::any& b) { return equal_value<bool>(a, b); },
    .compare = [](const std::any& a, const std::any& b) { return compare_value<bool>(a, b); }
};

inline const TypeHandler kIntHandler = {
    .name = "int",
    .output = [](std::ostream& os, const std::any& v) { output_value<int>(os, v, "int"); },
    .to_string = [](std::ostream& os, std::string_view k, const std::any& v) { to_string_value<int>(os, k, v); },
    .equal = [](const std::any& a, const std::any& b) { return equal_value<int>(a, b); },
    .compare = [](const std::any& a, const std::any& b) { return compare_value<int>(a, b); }
};

inline const TypeHandler kInt64Handler = {
    .name = "int64",
    .output = [](std::ostream& os, const std::any& v) { output_value<int64_t>(os, v, "int64"); },
    .to_string = [](std::ostream& os, std::string_view k, const std::any& v) { to_string_value<int64_t>(os, k, v); },
    .equal = [](const std::any& a, const std::any& b) { return equal_value<int64_t>(a, b); },
    .compare = [](const std::any& a, const std::any& b) { return compare_value<int64_t>(a, b); }
};

inline const TypeHandler kDoubleHandler = {
    .name = "double",
    .output = [](std::ostream& os, const std::any& v) { output_value<double>(os, v, "double"); },
    .to_string = [](std::ostream& os, std::string_view k, const std::any& v) { to_string_value<double>(os, k, v); },
    .equal = [](const std::any& a, const std::any& b) { return equal_value<double>(a, b); },
    .compare = [](const std::any& a, const std::any& b) { return compare_value<double>(a, b); }
};

// C++20: 使用 constexpr 函数提高性能
constexpr ParamType getParamType(const std::type_info& type) noexcept {
    if (type == typeid(bool)) return ParamType::Bool;
    if (type == typeid(int)) return ParamType::Int;
    if (type == typeid(int64_t)) return ParamType::Int64;
    if (type == typeid(double)) return ParamType::Double;
    return ParamType::Unknown;
}

inline ParamType getParamType(const std::any& value) noexcept {
    return getParamType(value.type());
}

// C++20: 使用 constexpr 查找表
inline const TypeHandler* getHandler(ParamType type) noexcept {
    switch (type) {
        case ParamType::Bool: return &kBoolHandler;
        case ParamType::Int: return &kIntHandler;
        case ParamType::Int64: return &kInt64Handler;
        case ParamType::Double: return &kDoubleHandler;
        default: return nullptr;
    }
}

} // namespace detail

// C++20: 使用 ranges 和现代语法简化输出
std::ostream& operator<<(std::ostream& os, const Params& params) {
    os << "Params[";
    
    // C++20: 使用结构化绑定
    for (const auto& [key, value] : params.params_) {
        os << key;
        auto type = detail::getParamType(value);
        
        if (auto* handler = detail::getHandler(type); handler) {
            handler->output(os, value);
        } else {
            os << "=Unsupported" << detail::kSplitStr;
        }
    }
    
    os << "]";
    return os;
}

bool Params::support(const std::any& value) noexcept {
    return detail::getParamType(value) != detail::ParamType::Unknown;
}

std::string Params::type(const std::string& key) const {
    auto it = params_.find(key);
    if (it == params_.end()) {
        throw std::out_of_range(fmt::format("Param not found: {}", key));
    }
    
    auto param_type = detail::getParamType(it->second);
    if (auto* handler = detail::getHandler(param_type); handler) {
        return std::string(handler->name);
    }
    return "unknown";
}

// C++20: 使用 ranges 简化
StringVec Params::keys() const {
    StringVec result;
    result.reserve(params_.size());
    
    // C++20: 使用 views::keys
    for (const auto& key : params_ | std::views::keys) {
        result.push_back(key);
    }
    
    return result;
}

std::string Params::to_string() const {
    std::ostringstream ss;
    
    for (const auto& [key, value] : params_) {
        auto type = detail::getParamType(value);
        
        if (auto* handler = detail::getHandler(type); handler) {
            handler->to_string(ss, key, value);
        } else {
            ss << key << "=Unsupported" << detail::kSplitStr;
        }
    }
    
    return ss.str();
}

// C++20: 实现三路比较运算符
std::weak_ordering Params::operator<=>(const Params& other) const noexcept {
    // 首先比较大小
    if (size() < other.size()) return std::weak_ordering::less;
    if (size() > other.size()) return std::weak_ordering::greater;
    
    // 逐个比较元素
    auto it_lhs = begin();
    auto it_rhs = other.begin();
    
    while (it_lhs != end() && it_rhs != other.end()) {
        // 比较键
        if (it_lhs->first < it_rhs->first) {
            return std::weak_ordering::less;
        }
        if (it_lhs->first > it_rhs->first) {
            return std::weak_ordering::greater;
        }
        
        // 比较类型
        auto type_lhs = detail::getParamType(it_lhs->second);
        auto type_rhs = detail::getParamType(it_rhs->second);
        
        if (type_lhs < type_rhs) {
            return std::weak_ordering::less;
        }
        if (type_lhs > type_rhs) {
            return std::weak_ordering::greater;
        }
        
        // 如果类型未知，无法比较
        if (type_lhs == detail::ParamType::Unknown) {
            return std::weak_ordering::equivalent;
        }
        
        // 比较值
        if (auto* handler = detail::getHandler(type_lhs); handler) {
            auto cmp = handler->compare(it_lhs->second, it_rhs->second);
            if (cmp != 0) {
                return cmp;
            }
        }
        
        ++it_lhs;
        ++it_rhs;
    }
    
    return std::weak_ordering::equivalent;
}

// C++20: operator== 的实现（使用新的比较逻辑）
bool Params::operator==(const Params& other) const noexcept {
    // 快速检查：大小不同则不相等
    if (size() != other.size()) {
        return false;
    }
    
    // 逐个比较
    auto it_lhs = begin();
    auto it_rhs = other.begin();
    
    while (it_lhs != end() && it_rhs != other.end()) {
        // 键必须相同
        if (it_lhs->first != it_rhs->first) {
            return false;
        }
        
        // 类型必须相同
        auto type_lhs = detail::getParamType(it_lhs->second);
        auto type_rhs = detail::getParamType(it_rhs->second);
        
        if (type_lhs != type_rhs) {
            return false;
        }
        
        // 未知类型视为不相等
        if (type_lhs == detail::ParamType::Unknown) {
            return false;
        }
        
        // 值必须相等
        if (auto* handler = detail::getHandler(type_lhs); handler) {
            if (!handler->equal(it_lhs->second, it_rhs->second)) {
                return false;
            }
        }
        
        ++it_lhs;
        ++it_rhs;
    }
    
    return true;
}

} // namespace sequoia::utils