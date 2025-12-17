---
description: C++ Cursor Rules (Optimized)
globs: **/*.c,**/*.cc,**/*.cpp,**/*.cxx,**/*.h,**/*.hpp,CMakeLists.txt,*.cmake,conanfile.*,Makefile
alwaysApply: true
---

# C++ Cursor 编码规范（优化版）

> 本规则用于 Cursor / AI 编程辅助工具，作为 **强约束的团队级 C++ 编码与设计规范**。
> 目标：
> - 最大化代码风格一致性
> - 降低重复与歧义规则
> - 可自动化（clang-format / clang-tidy）
> - 适配不同项目形态（嵌入式 / 服务端 / 跨平台）

---

## 1. 基本原则（强制）

- 所有代码、注释、文档 **统一使用英文**
- 所有变量、函数、参数、返回值 **必须显式声明类型**
- 遵循 **One Definition Rule (ODR)**
- 禁止函数体内无意义空行
- 禁止未使用变量、参数、include

---

## 2. 命名规范（强制）

### 2.1 类型命名

- 类 / 结构体 / 枚举：`PascalCase`

```cpp
class FileManager;
struct UserInfo;
enum class ErrorCode;
```

### 2.2 函数与方法

- 使用 `PascalCase`
- 必须以 **动词** 开头

```cpp
void LoadConfig();
bool IsValid();
```

### 2.3 变量命名

- 局部变量 / 参数：`snake_case`
- 成员变量：`snake_case_`

```cpp
int file_count;
int max_retry_;
```

### 2.4 Getter / Setter

- Getter：与成员变量同名（无后缀）
- Setter：`set_` + 变量名

```cpp
int timeout() const;
void set_timeout(int timeout);
```

### 2.5 常量与宏

- 使用 `ALL_CAPS`
- 禁止魔法数字

```cpp
constexpr int MAX_BUFFER_SIZE = 4096;
```

---

## 3. 格式与排版（自动化）

- 使用 **clang-format 强制格式化**
- 禁止手写风格调整

### 3.1 clang-format 推荐配置

```yaml
Language:        Cpp
BasedOnStyle:    Google
IndentWidth:     4
UseTab:          Never
ColumnLimit:     100
BreakBeforeBraces: Attach
```

### 3.2 基本规则

- 使用空格缩进（4 spaces）
- 左大括号不换行
- `if / for / while` 后必须有空格

```cpp
if (condition) {
    ExecuteTask();
}
```

---

## 4. 注释规范（强制）

### 4.1 公共 API

- 使用 **Doxygen 风格注释**

```cpp
/// @brief Load configuration from file
/// @param path Absolute file path
/// @return true if success
bool LoadConfig(const std::string& path);
```

### 4.2 行内注释

- 仅用于解释 **为什么**，而非 **做什么**
- 禁止无意义注释

---

## 5. 函数设计规则（强制）

- 单一职责
- 函数长度 ≤ 20 行
- 避免嵌套，使用 early-return

```cpp
if (!is_ready) {
    return false;
}
```

- 参数 ≥ 3 时，必须使用结构体封装

---

## 6. 数据与类型设计

- 禁止滥用 primitive type
- 优先使用强类型封装
- 不可变优先（immutability）

```cpp
struct RequestContext {
    std::string user_id;
    int timeout_ms;
};
```

- 可空值：`std::optional`
- 编译期常量：`constexpr`

---

## 7. 类设计（SOLID）

- 单一职责
- 组合优于继承
- 成员变量必须私有

### 7.1 规模约束

- ≤ 200 行实现
- ≤ 10 public methods
- ≤ 10 成员变量

### 7.2 资源管理

- 优先 Rule of Zero
- 否则 Rule of Five

---

## 8. 错误处理

- **异常仅用于不可预期错误**
- 可预期失败：
  - `std::optional`
  - `std::expected`
  - error code

```cpp
std::optional<User> FindUser(UserId id);
```

---

## 9. 内存管理（强制）

- 禁止裸 `new / delete`
- 使用 RAII
- 优先：
  - `std::unique_ptr`
  - `std::shared_ptr`（慎用）

---

## 10. 并发与线程安全

- 使用 `std::mutex` + `std::lock_guard`
- 原子操作使用 `std::atomic`
- 禁止数据竞争

---

## 11. 测试规范

- Arrange – Act – Assert
- 每个 public 函数必须有单元测试

```cpp
// Given
// When
// Then
```

---

## 12. 项目结构

```text
project/
├── include/
├── src/
├── test/
├── lib/
└── doc/
```

- 头文件与实现分离
- 头文件必须自包含

---

## 13. clang-tidy（强烈推荐）

### 13.1 推荐 checks

```yaml
Checks: >
  -*,
  clang-analyzer-*,
  modernize-*,
  performance-*,
  readability-*,
  bugprone-*
```

- CI 中强制执行
- 本地开发启用自动修复

---

## 14. 场景适配补充

### 14.1 嵌入式

- 禁用异常 / RTTI
- 避免动态分配
- 遵循 MISRA / CERT

### 14.2 服务端 / 高并发

- 强调线程安全
- 异步 / task-based 设计
- 明确生命周期管理

### 14.3 跨平台

- 使用标准库
- 条件编译隔离平台差异
- UTF-8 编码

---

## 15. Cursor 行为约束（AI 专用）

- **不得生成违反以上规则的代码**
- 自动补全必须符合命名与格式规范
- 新增类型必须配套注释
- 禁止生成未测试的 public API

---

**This rule is authoritative. Do not deviate unless explicitly instructed.**