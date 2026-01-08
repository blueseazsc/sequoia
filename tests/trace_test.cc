#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <sequoia/utils/trace/trace.h>
#include <thread>
#include <chrono>

// 测试函数：使用作用域跟踪
void TracedFunction() {
    T_SCOPED;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

// 测试函数：使用命名作用域跟踪
void NamedTracedFunction() {
    T_SCOPED_NAME("CustomNamedFunction");
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

// 测试函数：嵌套跟踪
void NestedTrace() {
    T_SCOPED_NAME("OuterScope");
    {
        T_SCOPED_NAME("InnerScope1");
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    {
        T_SCOPED_NAME("InnerScope2");
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

// 测试帧标记
void FrameFunction() {
    T_FRAME_MARK;
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

// 测试命名帧标记
void NamedFrameFunction() {
    T_FRAME_MARK_NAME("GameFrame");
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

class TracedClass {
public:
    TracedClass() {
        T_SCOPED_NAME("TracedClass::Constructor");
    }

    ~TracedClass() {
        T_SCOPED_NAME("TracedClass::Destructor");
    }

    void TracedMethod() {
        T_SCOPED_NAME("TracedClass::TracedMethod");
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
};

TEST_CASE("Trace Test") {
    // 初始化跟踪系统
    T_STARTUP;

    // 1. 测试 T_STARTUP 和 T_SHUTDOWN
    SUBCASE("Startup and Shutdown") {
        CHECK_NOTHROW(T_SHUTDOWN);
        CHECK_NOTHROW(T_STARTUP);
    }

    // 2. 测试基本功能
    SUBCASE("Basic Functionality") {
        CHECK_NOTHROW(TracedFunction());
        CHECK_NOTHROW(NamedTracedFunction());
        CHECK_NOTHROW(NestedTrace());
        CHECK_NOTHROW(FrameFunction());
        CHECK_NOTHROW(NamedFrameFunction());
    }

    // 3. 测试多个连续作用域
    SUBCASE("Multiple Sequential Scopes") {
        {
            T_SCOPED_NAME("Scope1");
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        {
            T_SCOPED_NAME("Scope2");
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        {
            T_SCOPED_NAME("Scope3");
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        CHECK(true);
    }

    // 4. 测试循环中的跟踪
    SUBCASE("Trace in Loop") {
        for (int i = 0; i < 5; ++i) {
            T_SCOPED_NAME("LoopIteration");
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        CHECK(true);
    }

    // 5. 测试多个帧标记
    SUBCASE("Multiple Frame Marks") {
        for (int frame = 0; frame < 3; ++frame) {
            T_FRAME_MARK;
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        for (int frame = 0; frame < 3; ++frame) {
            T_FRAME_MARK_NAME("TestFrame");
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        CHECK(true);
    }

    // 6. 测试条件编译（所有宏都存在）
    SUBCASE("Conditional Compilation") {
        {
            T_SCOPED;
        }
        {
            T_SCOPED_NAME("ConditionalTest");
        }
        T_FRAME_MARK;
        T_FRAME_MARK_NAME("ConditionalFrame");
        CHECK(true);
    }

    // 7. 测试 Lambda 函数中的跟踪
    SUBCASE("Trace in Lambda") {
        auto traced_lambda = []() {
            T_SCOPED_NAME("LambdaScope");
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            return 42;
        };

        int result = traced_lambda();
        CHECK(result == 42);
    }

    // 8. 测试异常安全性
    SUBCASE("Exception Safety") {
        try {
            T_SCOPED_NAME("ExceptionScope");
            throw std::runtime_error("Test exception");
        } catch (const std::exception& e) {
            CHECK(std::string(e.what()) == "Test exception");
        }
    }

    // 9. 测试类方法中的跟踪
    SUBCASE("Class Methods") {
        CHECK_NOTHROW({
            TracedClass obj;
            obj.TracedMethod();
        });
    }

    // 10. 测试性能开销
    SUBCASE("Performance Overhead") {
        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < 1000; ++i) {
            T_SCOPED_NAME("PerformanceTest");
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        // 确保性能开销在可接受范围内（即使启用了跟踪）
        CHECK(duration.count() < 5000); // 少于 5 秒
    }

    // 11. 测试完整生命周期
    SUBCASE("Full Lifecycle") {
        {
            T_SCOPED_NAME("LifecycleTest");
            TracedFunction();
            NamedTracedFunction();
            T_FRAME_MARK;
        }
        CHECK(true);
    }

    // 清理跟踪系统
    T_SHUTDOWN;
}
