#include <gtest/gtest.h>
#include "RType/Logger.hpp"
#include "RType/LogLevel.hpp"
#include <filesystem>
#include <fstream>
#include <thread>
#include <chrono>

using namespace rtp::log;

// ==================== LOG LEVEL TESTS ====================

TEST(LogLevelTest, EnumValues) {
    EXPECT_EQ(static_cast<uint8_t>(Level::Debug), 0);
    EXPECT_EQ(static_cast<uint8_t>(Level::Info), 1);
    EXPECT_EQ(static_cast<uint8_t>(Level::Warning), 2);
    EXPECT_EQ(static_cast<uint8_t>(Level::Error), 3);
    EXPECT_EQ(static_cast<uint8_t>(Level::Fatal), 4);
    EXPECT_EQ(static_cast<uint8_t>(Level::None), 5);
}

TEST(LogLevelTest, ToString) {
    EXPECT_EQ(toString(Level::Debug), "Debug");
    EXPECT_EQ(toString(Level::Info), "Info");
    EXPECT_EQ(toString(Level::Warning), "Warning");
    EXPECT_EQ(toString(Level::Error), "Error");
    EXPECT_EQ(toString(Level::Fatal), "Fatal");
    EXPECT_EQ(toString(Level::None), "None");
}

TEST(LogLevelTest, Ordering) {
    EXPECT_LT(Level::Debug, Level::Info);
    EXPECT_LT(Level::Info, Level::Warning);
    EXPECT_LT(Level::Warning, Level::Error);
    EXPECT_LT(Level::Error, Level::Fatal);
    EXPECT_LT(Level::Fatal, Level::None);
}

// ==================== LOGGER CONFIGURATION ====================

TEST(LoggerConfigTest, ConfigureValidPath) {
    std::filesystem::path testLog = "logs/test_logger.log";
    std::filesystem::create_directories(testLog.parent_path());
    
    auto result = configure(testLog.string());
    EXPECT_TRUE(result.has_value());
    
    // Cleanup
    if (std::filesystem::exists(testLog)) {
        std::filesystem::remove(testLog);
    }
}

TEST(LoggerConfigTest, ConfigureCreatesDirectory) {
    std::filesystem::path testLog = "logs/test_subdir/test.log";
    
    // Remove if exists
    if (std::filesystem::exists(testLog.parent_path())) {
        std::filesystem::remove_all(testLog.parent_path());
    }
    
    auto result = configure(testLog.string());
    EXPECT_TRUE(result.has_value());
    
    // Cleanup
    if (std::filesystem::exists("logs/test_subdir")) {
        std::filesystem::remove_all("logs/test_subdir");
    }
}

// ==================== BASIC LOGGING TESTS ====================

TEST(LoggerTest, DebugLog) {
    ASSERT_NO_THROW(debug("Debug message: {}", 42));
}

TEST(LoggerTest, InfoLog) {
    ASSERT_NO_THROW(info("Info message: {}", "test"));
}

TEST(LoggerTest, WarningLog) {
    ASSERT_NO_THROW(warning("Warning message: {}", 3.14));
}

TEST(LoggerTest, ErrorLog) {
    ASSERT_NO_THROW(error("Error message: {}", "error"));
}

TEST(LoggerTest, FatalLog) {
    ASSERT_NO_THROW(fatal("Fatal message: {}", 999));
}

// ==================== FORMAT STRING TESTS ====================

TEST(LoggerFormatTest, SimpleString) {
    ASSERT_NO_THROW(info("Simple message without format"));
}

TEST(LoggerFormatTest, SingleArgument) {
    ASSERT_NO_THROW(info("Single arg: {}", 123));
}

TEST(LoggerFormatTest, MultipleArguments) {
    ASSERT_NO_THROW(info("Multiple args: {} {} {}", 1, 2.5f, "three"));
}

TEST(LoggerFormatTest, FormattedNumbers) {
    ASSERT_NO_THROW(info("Hex: {:#x}, Float: {:.2f}", 255, 3.14159));
}

TEST(LoggerFormatTest, StringTypes) {
    std::string str = "std::string";
    const char* cstr = "C string";
    ASSERT_NO_THROW(info("Strings: {} and {}", str, cstr));
}

// ==================== TYPE SAFETY TESTS ====================

TEST(LoggerTypeSafetyTest, IntegerTypes) {
    int8_t i8 = -128;
    uint8_t u8 = 255;
    int16_t i16 = -32768;
    uint16_t u16 = 65535;
    int32_t i32 = -2147483648;
    uint32_t u32 = 4294967295;
    
    ASSERT_NO_THROW(info("Integers: {} {} {} {} {} {}", i8, u8, i16, u16, i32, u32));
}

TEST(LoggerTypeSafetyTest, FloatingPointTypes) {
    float f = 3.14f;
    double d = 2.71828;
    
    ASSERT_NO_THROW(info("Floats: {} {}", f, d));
}

TEST(LoggerTypeSafetyTest, BooleanType) {
    bool t = true;
    bool f = false;
    
    ASSERT_NO_THROW(info("Booleans: {} {}", t, f));
}

// ==================== THREAD SAFETY TESTS ====================

TEST(LoggerThreadSafetyTest, ConcurrentLogging) {
    constexpr int NUM_THREADS = 10;
    constexpr int LOGS_PER_THREAD = 100;
    
    std::vector<std::thread> threads;
    
    for (int t = 0; t < NUM_THREADS; ++t) {
        threads.emplace_back([t]() {
            for (int i = 0; i < LOGS_PER_THREAD; ++i) {
                info("Thread {} log {}", t, i);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    // If no crash occurred, thread safety is working
    SUCCEED();
}

TEST(LoggerThreadSafetyTest, MixedLevelsConcurrent) {
    std::vector<std::thread> threads;
    
    threads.emplace_back([]() {
        for (int i = 0; i < 50; ++i) debug("Debug {}", i);
    });
    
    threads.emplace_back([]() {
        for (int i = 0; i < 50; ++i) info("Info {}", i);
    });
    
    threads.emplace_back([]() {
        for (int i = 0; i < 50; ++i) warning("Warning {}", i);
    });
    
    threads.emplace_back([]() {
        for (int i = 0; i < 50; ++i) error("Error {}", i);
    });
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    SUCCEED();
}

// ==================== FILE OUTPUT TESTS ====================

TEST(LoggerFileTest, WritesToFile) {
    std::filesystem::path testLog = "logs/test_file_output.log";
    std::filesystem::create_directories(testLog.parent_path());
    
    // Remove old log if exists
    if (std::filesystem::exists(testLog)) {
        std::filesystem::remove(testLog);
    }
    
    auto result = configure(testLog.string());
    ASSERT_TRUE(result.has_value());
    
    info("Test message for file output");
    
    // Small delay to ensure write completes
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Check file exists
    EXPECT_TRUE(std::filesystem::exists(testLog));
    
    // Check file has content
    std::ifstream file(testLog);
    ASSERT_TRUE(file.is_open());
    
    std::string line;
    bool found = false;
    while (std::getline(file, line)) {
        if (line.find("Test message for file output") != std::string::npos) {
            found = true;
            break;
        }
    }
    
    EXPECT_TRUE(found);
    
    // Cleanup
    file.close();
    if (std::filesystem::exists(testLog)) {
        std::filesystem::remove(testLog);
    }
}

TEST(LoggerFileTest, MultipleMessages) {
    std::filesystem::path testLog = "logs/test_multiple.log";
    std::filesystem::create_directories(testLog.parent_path());
    
    if (std::filesystem::exists(testLog)) {
        std::filesystem::remove(testLog);
    }
    
    auto result = configure(testLog.string());
    ASSERT_TRUE(result.has_value());
    
    info("Message 1");
    info("Message 2");
    info("Message 3");
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::ifstream file(testLog);
    ASSERT_TRUE(file.is_open());
    
    int lineCount = 0;
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("Message") != std::string::npos) {
            ++lineCount;
        }
    }
    
    EXPECT_EQ(lineCount, 3);
    
    file.close();
    if (std::filesystem::exists(testLog)) {
        std::filesystem::remove(testLog);
    }
}

// ==================== EDGE CASES ====================

TEST(LoggerEdgeCaseTest, EmptyMessage) {
    ASSERT_NO_THROW(info(""));
}

TEST(LoggerEdgeCaseTest, LongMessage) {
    std::string longMsg(1000, 'x');
    ASSERT_NO_THROW(info("{}", longMsg));
}

TEST(LoggerEdgeCaseTest, SpecialCharacters) {
    ASSERT_NO_THROW(info("Special chars with placeholders: {}", "test"));
}

TEST(LoggerEdgeCaseTest, UnicodeCharacters) {
    ASSERT_NO_THROW(info("Unicode: 日本語 🎮 ñ"));
}

// ==================== STRESS TESTS ====================

TEST(LoggerStressTest, ManyMessagesSequential) {
    constexpr int NUM_MESSAGES = 1000;
    
    for (int i = 0; i < NUM_MESSAGES; ++i) {
        info("Stress test message {}", i);
    }
    
    SUCCEED();
}

TEST(LoggerStressTest, RapidLogging) {
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 500; ++i) {
        info("Rapid log {}", i);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should complete in reasonable time (< 5 seconds)
    EXPECT_LT(duration.count(), 5000);
}

// ==================== FORMAT WRAPPER TESTS ====================

TEST(LoggerWrapperTest, LogFmtConstruction) {
    // LogFmt should work with const char*
    ASSERT_NO_THROW(info("Test {}", 42));
    
    // LogFmt requires compile-time constant format strings
    constexpr auto fmt = "Test {}";
    ASSERT_NO_THROW(info(fmt, 42));
}

// ==================== ALL LEVELS TEST ====================

TEST(LoggerIntegrationTest, AllLevelsSequential) {
    debug("Debug level message");
    info("Info level message");
    warning("Warning level message");
    error("Error level message");
    fatal("Fatal level message");
    
    SUCCEED();
}

TEST(LoggerIntegrationTest, MixedFormats) {
    info("Integer: {}, Float: {:.2f}, String: {}", 42, 3.14159, "test");
    warning("Hex: {:#x}, Binary: {:#b}", 255, 15);
    error("Boolean: {}, Char: {}", true, 'A');
    
    SUCCEED();
}
