#include "Log.hpp"

#ifdef _WIN32

#include <windows.h>

#else
#include <unistd.h>
#endif

std::shared_ptr<Log> Log::coreLogger = nullptr;

void Log::init() {
    coreLogger = std::make_shared<Log>();
}

std::shared_ptr<Log> &Log::getCoreLogger() {
    return coreLogger;
}

void Log::setLogLevel(LogLevel level) {
    logLevel = level;
}

void Log::log(LogLevel level, const std::string &message) {
    if (level >= logLevel || level == LogLevel::Message) {
        applyColor(level);
        std::cout << "[" << getCurrentTime() << "/" << getLogLevelString(level) << "]: "
                  << message << std::endl;
        resetColor();

        if (level == LogLevel::Fatal) {
            exit(1);
        }
    }
}

std::string Log::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&in_time_t), "%H:%M:%S");
    return oss.str();
}

// Get log level as string
constexpr std::string Log::getLogLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::Message:
            return "Message";
        case LogLevel::Trace:
            return "Trace";
        case LogLevel::Info:
            return "Info";
        case LogLevel::Warn:
            return "Warn";
        case LogLevel::Error:
            return "Error";
        case LogLevel::Fatal:
            return "Fatal";
        default:
            return "Unknown";
    }
}

void Log::applyColor(LogLevel level) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    switch (level) {
        case LogLevel::Message:
            SetConsoleTextAttribute(hConsole, 10);
            break; // green
        case LogLevel::Trace:
            SetConsoleTextAttribute(hConsole, 7);
            break;    // White
        case LogLevel::Info:
            SetConsoleTextAttribute(hConsole, 13);
            break;   // Violet (Magenta)
        case LogLevel::Warn:
            SetConsoleTextAttribute(hConsole, 14);
            break;   // Yellow
        case LogLevel::Error:
            SetConsoleTextAttribute(hConsole, 12);
            break;   // Red
        case LogLevel::Fatal:
            SetConsoleTextAttribute(hConsole, 12 | FOREGROUND_INTENSITY);
            break; // Bold Red
    }
#else
    switch (level) {
        case LogLevel::Message  std::cout << "\033[32m"; break;  // Green
        case LogLevel::Trace: std::cout << "\033[37m"; break;  // White
        case LogLevel::Info:  std::cout << "\033[35m"; break;  // Violet (Magenta)
        case LogLevel::Warn:  std::cout << "\033[33m"; break;  // Yellow
        case LogLevel::Error: std::cout << "\033[31m"; break;  // Red
        case LogLevel::Fatal: std::cout << "\033[1;31m"; break; // Bold Red
    }
#endif
}

void Log::resetColor() {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 7);
#else
    std::cout << "\033[0m";
#endif
}
