#pragma once

#include <Atlas.hpp>
#include <crow.h>

class CrowLogger : public crow::ILogHandler {
public:
    void log(std::string message, crow::LogLevel crowLevel) override {
        LogLevel customLevel = mapLogLevel(crowLevel);

        Log::getCoreLogger()->log(customLevel, message);
    }

private:
    static LogLevel mapLogLevel(const crow::LogLevel crowLevel) {
        switch (crowLevel) {
            case crow::LogLevel::Debug: return LogLevel::Trace;
            case crow::LogLevel::Info: return LogLevel::Info;
            case crow::LogLevel::Warning: return LogLevel::Warn;
            case crow::LogLevel::Error: return LogLevel::Error;
            default: return LogLevel::Message;
        }
    }
};