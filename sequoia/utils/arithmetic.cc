#include "arithmetic.h"

#include <iconv.h>
#include <memory>

namespace sequoia::utils {

// 显式模板实例化（C++20 中移除 constexpr）
template double roundEx(double value, int32_t ndigits);
template float roundEx(float value, int32_t ndigits);

template double ceilEx(double value, int32_t ndigits);
template float ceilEx(float value, int32_t ndigits);

template double floorEx(double value, int32_t ndigits);
template float floorEx(float value, int32_t ndigits);

namespace detail {

// RAII 包装器：自动管理 iconv 资源生命周期
class IconvConverter {
public:
    IconvConverter(const char* to_code, const char* from_code) noexcept
        : cd_(iconv_open(to_code, from_code)) {}
    
    ~IconvConverter() noexcept {
        if (is_valid()) {
            iconv_close(cd_);
        }
    }
    
    // 禁用拷贝，允许移动（C++20 移动语义优化）
    IconvConverter(const IconvConverter&) = delete;
    IconvConverter& operator=(const IconvConverter&) = delete;
    
    IconvConverter(IconvConverter&& other) noexcept : cd_(other.cd_) {
        other.cd_ = reinterpret_cast<iconv_t>(-1);
    }
    
    IconvConverter& operator=(IconvConverter&& other) noexcept {
        if (this != &other) {
            if (is_valid()) {
                iconv_close(cd_);
            }
            cd_ = other.cd_;
            other.cd_ = reinterpret_cast<iconv_t>(-1);
        }
        return *this;
    }
    
    [[nodiscard]] bool is_valid() const noexcept {
        return cd_ != reinterpret_cast<iconv_t>(-1);
    }
    
    [[nodiscard]] size_t convert(char** inbuf, size_t* inleft, 
                                   char** outbuf, size_t* outleft) noexcept {
        return iconv(cd_, inbuf, inleft, outbuf, outleft);
    }
    
private:
    iconv_t cd_;
};

// 通用字符编码转换函数（使用 string_view 减少拷贝）
[[nodiscard]] std::string convert_encoding(std::string_view input,
                                            const char* to_encoding,
                                            const char* from_encoding,
                                            size_t buffer_multiplier) {
    if (input.empty()) {
        return {};  // C++20: 空大括号初始化更清晰
    }

    IconvConverter converter(to_encoding, from_encoding);
    if (!converter.is_valid()) {
        return std::string{input};  // 转换失败，返回原字符串
    }

    // 准备输入缓冲区（iconv 需要非 const 指针，但不会修改数据）
    char* in_buf = const_cast<char*>(input.data());
    size_t in_left = input.size();
    
    // 分配输出缓冲区
    std::string result(input.size() * buffer_multiplier, '\0');
    char* out_buf = result.data();
    size_t out_left = result.size();

    // 执行字符编码转换
    const size_t ret = converter.convert(&in_buf, &in_left, &out_buf, &out_left);

    if (ret == static_cast<size_t>(-1)) {
        return std::string{input};  // 转换失败，返回原字符串
    }

    // 调整结果字符串大小至实际使用的大小
    result.resize(result.size() - out_left);
    return result;
}

} // anonymous namespace

std::string utf8_to_gbk(const std::string& str) {
    // UTF-8 转 GBK 通常需要 1 倍空间（中文字符从 3 字节变为 2 字节）
    return detail::convert_encoding(str, "GBK", "UTF-8", 1);
}

std::string gbk_to_utf8(const std::string& str) {
    // GBK 转 UTF-8 通常需要 2 倍空间（中文字符从 2 字节变为 3 字节）
    return detail::convert_encoding(str, "UTF-8", "GBK", 2);
}

} // namespace sequoia::utils