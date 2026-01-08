
#pragma once

#include <string>
#include <map>
#include <any>
#include <concepts>
#include <ranges>
#include <compare>
#include <fmt/format.h>
#include <sequoia/utils/types.h>
#include <sequoia/utils/log/log.h>

namespace sequoia::utils {

// C++20 concept: 定义支持的参数类型
template<typename T>
concept SupportedParamType = std::same_as<T, bool> || 
                              std::same_as<T, int> || 
                              std::same_as<T, int64_t> || 
                              std::same_as<T, double>;

// C++20 concept: 可转换的整数类型（int 和 int64_t 互相兼容）
template<typename T>
concept IntegerParamType = std::same_as<T, int> || std::same_as<T, int64_t>;

class Params {
public:
    using ParamMap = std::map<std::string, std::any>;
    using const_iterator = ParamMap::const_iterator;
    friend std::ostream& operator<<(std::ostream& os, const Params& params);
    friend std::any get_any(const Params&, const std::string&);
    friend void set_any(Params&, const std::string&, const std::any&);

    // 检查类型是否支持
    static constexpr bool is_supported_type(const std::type_info& type) noexcept {
        return type == typeid(bool) || type == typeid(int) || 
               type == typeid(int64_t) || type == typeid(double);
    }
    
    static bool support(const std::any& value) noexcept;

public:
    // 使用 ranges 返回键的视图
    auto keys_view() const {
        return params_ | std::views::keys;
    }
    
    StringVec keys() const;
    std::string to_string() const;

    [[nodiscard]] bool have(const std::string& key) const noexcept {
        return params_.contains(key);  // C++20: contains 替代 find
    }

    [[nodiscard]] size_t size() const noexcept {
        return params_.size();
    }
    
    [[nodiscard]] bool empty() const noexcept {
        return params_.empty();
    }

    std::string type(const std::string& key) const;

    // 使用 concept 约束的模板方法
    template <typename ValueType>
        requires SupportedParamType<ValueType>
    [[nodiscard]] ValueType get(const std::string& key) const;

    template <typename ValueType>
        requires SupportedParamType<ValueType>
    [[nodiscard]] ValueType try_get(const std::string& key, const ValueType& default_value) const noexcept;

    template <typename ValueType>
        requires SupportedParamType<ValueType>
    void set(const std::string& key, const ValueType& value);
    
    // 删除参数
    bool remove(const std::string& key) noexcept {
        return params_.erase(key) > 0;
    }
    
    // 清空所有参数
    void clear() noexcept {
        params_.clear();
    }

public:
    [[nodiscard]] const_iterator begin() const noexcept {
        return params_.begin();
    }

    [[nodiscard]] const_iterator end() const noexcept {
        return params_.end();
    }
    
    // C++20: 三路比较运算符
    [[nodiscard]] std::weak_ordering operator<=>(const Params& other) const noexcept;
    [[nodiscard]] bool operator==(const Params& other) const noexcept;

private:
    ParamMap params_;
};

// 模板实现：使用 concept 约束
template <typename ValueType>
    requires SupportedParamType<ValueType>
ValueType Params::get(const std::string& key) const {
    auto it = params_.find(key);
    if (it == params_.end()) {
        throw std::out_of_range(fmt::format("Param not found: {}", key));
    }
    
    try {
        return std::any_cast<ValueType>(it->second);
    } catch (const std::bad_any_cast& e) {
        // 尝试整数类型之间的转换（int <-> int64_t）
        if constexpr (IntegerParamType<ValueType>) {
            if (it->second.type() == typeid(int) && std::is_same_v<ValueType, int64_t>) {
                return static_cast<int64_t>(std::any_cast<int>(it->second));
            } else if (it->second.type() == typeid(int64_t) && std::is_same_v<ValueType, int>) {
                return static_cast<int>(std::any_cast<int64_t>(it->second));
            }
        }
        
        throw std::runtime_error(fmt::format(
            "Failed convert param {} from {} to {}: {}", 
            key, it->second.type().name(), typeid(ValueType).name(), 
            e.what()));
    }
}

template <typename ValueType>
    requires SupportedParamType<ValueType>
ValueType Params::try_get(const std::string& key, 
                               const ValueType& default_value) const noexcept {
    try {
        return get<ValueType>(key);
    } catch (...) {
        return default_value;
    }
}

template <typename ValueType>
    requires SupportedParamType<ValueType>
void Params::set(const std::string& key, const ValueType& value) {
    // 如果参数不存在，直接设置
    if (!have(key)) {
        params_[key] = value;
        return;
    }
    
    // 检查类型兼容性
    const auto& existing_type = params_[key].type();
    const auto& new_type = typeid(ValueType);
    
    if (existing_type != new_type) {
        // 允许 int 和 int64_t 之间的转换
        if constexpr (IntegerParamType<ValueType>) {
            if (existing_type == typeid(int) || existing_type == typeid(int64_t)) {
                // 允许转换
            } else {
                throw std::logic_error(fmt::format(
                    "Param {} type mismatch: {} != {}", 
                    key, existing_type.name(), new_type.name()));
            }
        } else {
            throw std::logic_error(fmt::format(
                "Param {} type mismatch: {} != {}", 
                key, existing_type.name(), new_type.name()));
        }
    }
    
    params_[key] = value;
}

// 获取原始 std::any 值（不使用特化，避免与 concept 冲突）
inline std::any get_any(const Params& params, const std::string& key) {
    auto it = params.params_.find(key);
    if (it == params.params_.end()) {
        throw std::out_of_range(fmt::format("Param not found: {}", key));
    }
    return it->second;
}

// 设置 std::any 值（不使用特化，避免与 concept 冲突）
inline void set_any(Params& params, const std::string& key, const std::any& value) {
    if (!params.have(key)) {
        params.params_[key] = value;
        return;
    }
    
    if (params.params_[key].type() != value.type()) {
        throw std::logic_error(fmt::format(
            "Param {} type mismatch: {} != {}", 
            key, params.params_[key].type().name(), value.type().name()));
    }
    
    params.params_[key] = value;
}

// C++20: 使用更现代的宏定义，增强类型安全
#define PARAMETERS_SUPPORT \
protected: \
    ::sequoia::utils::Params parameters_; \
public: \
    [[nodiscard]] const ::sequoia::utils::Params& GetParams() const noexcept { \
        return parameters_; \
    } \
    void SetParams(const ::sequoia::utils::Params& parameters) { \
        parameters_ = parameters; \
    } \
    [[nodiscard]] bool HaveParam(const std::string& key) const noexcept { \
        return parameters_.have(key); \
    } \
    template <typename ValueType> \
        requires ::sequoia::utils::SupportedParamType<ValueType> \
    ValueType GetParam(const std::string& key) const { \
        return parameters_.get<ValueType>(key); \
    } \
    template <typename ValueType> \
        requires ::sequoia::utils::SupportedParamType<ValueType> \
    ValueType TryGetParam(const std::string& key, \
                          const ValueType& default_value) const noexcept { \
        return parameters_.try_get<ValueType>(key, default_value); \
    } \
    template <typename ValueType> \
        requires ::sequoia::utils::SupportedParamType<ValueType> \
    void SetParam(const std::string& key, const ValueType& value) { \
        parameters_.set<ValueType>(key, value); \
    } \
    template <typename ValueType> \
        requires ::sequoia::utils::SupportedParamType<ValueType> \
    ValueType GetParamFromOther(const ::sequoia::utils::Params& other, \
                                const std::string& key, \
                                const ValueType& default_value) { \
        if (other.have(key)) { \
            SetParam<ValueType>(key, other.get<ValueType>(key)); \
        } else { \
            SetParam<ValueType>(key, default_value); \
        } \
        return GetParam<ValueType>(key); \
    } \
    [[nodiscard]] bool RemoveParam(const std::string& key) noexcept { \
        return parameters_.remove(key); \
    } \
    void ClearParams() noexcept { \
        parameters_.clear(); \
    }

#define PARAMETERS_SUPPORT_WITH_CHECK \
protected: \
    ::sequoia::utils::Params parameters_; \
    virtual void ParamChanged() = 0; \
    void CheckParam(const std::string& key) const { \
        BaseCheckParam(key); \
        DerivedCheckParam(key); \
    } \
    virtual void DerivedCheckParam(const std::string& key) const {} \
private: \
    void BaseCheckParam(const std::string& key) const; \
public: \
    [[nodiscard]] const ::sequoia::utils::Params& GetParams() const noexcept { \
        return parameters_; \
    } \
    void SetParams(const ::sequoia::utils::Params& parameters) { \
        parameters_ = parameters; \
        for (const auto& [key, value] : parameters_) { \
            CheckParam(key); \
        } \
        ParamChanged(); \
    } \
    [[nodiscard]] bool HaveParam(const std::string& key) const noexcept { \
        return parameters_.have(key); \
    } \
    template <typename ValueType> \
        requires ::sequoia::utils::SupportedParamType<ValueType> \
    ValueType GetParam(const std::string& key) const { \
        return parameters_.get<ValueType>(key); \
    } \
    template <typename ValueType> \
        requires ::sequoia::utils::SupportedParamType<ValueType> \
    ValueType TryGetParam(const std::string& key, \
                          const ValueType& default_value) const noexcept { \
        return parameters_.try_get<ValueType>(key, default_value); \
    } \
    template <typename ValueType> \
        requires ::sequoia::utils::SupportedParamType<ValueType> \
    void SetParam(const std::string& key, const ValueType& value) { \
        parameters_.set<ValueType>(key, value); \
        CheckParam(key); \
        ParamChanged(); \
    } \
    template <typename ValueType> \
        requires ::sequoia::utils::SupportedParamType<ValueType> \
    ValueType GetParamFromOther(const ::sequoia::utils::Params& other, \
                                const std::string& key, \
                                const ValueType& default_value) { \
        if (other.have(key)) { \
            SetParam<ValueType>(key, other.get<ValueType>(key)); \
        } else { \
            SetParam<ValueType>(key, default_value); \
        } \
        return GetParam<ValueType>(key); \
    } \
    [[nodiscard]] bool RemoveParam(const std::string& key) { \
        bool result = parameters_.remove(key); \
        if (result) { ParamChanged(); } \
        return result; \
    } \
    void ClearParams() { \
        parameters_.clear(); \
        ParamChanged(); \
    }

} // namespace sequoia::utils
