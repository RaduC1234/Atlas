#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <format>
#include <chrono>
#include <iomanip>

enum class LogLevel {
    Trace,
    Info,
    Warn,
    Error,
    Fatal
};

class Log {
private:
    static std::shared_ptr<Log> coreLogger;
    LogLevel logLevel = LogLevel::Trace;

public:
    static void init();

    static std::shared_ptr<Log> &getCoreLogger();

    void setLogLevel(LogLevel level);

    void log(LogLevel level, const std::string &message);

    template<typename... Args>
    void log(LogLevel level, const std::format_string<Args...> &formatString, Args &&... args) {
        log(level, std::format(formatString, std::forward<Args>(args)...));
    }

private:
    std::string getCurrentTime();

    std::string getLogLevelString(LogLevel level);

    void applyColor(LogLevel level);

    void resetColor();
};

#define AT_FATAL(...) { ::Log::getCoreLogger()->log(LogLevel::Fatal, __VA_ARGS__); }
#define AT_ERROR(...) { ::Log::getCoreLogger()->log(LogLevel::Error, __VA_ARGS__); }
#define AT_WARN(...)  { ::Log::getCoreLogger()->log(LogLevel::Warn, __VA_ARGS__); }
#define AT_INFO(...)  { ::Log::getCoreLogger()->log(LogLevel::Info, __VA_ARGS__); }
#define AT_TRACE(...) { ::Log::getCoreLogger()->log(LogLevel::Trace, __VA_ARGS__); }
