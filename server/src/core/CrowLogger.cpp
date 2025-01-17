#include "CrowLogger.hpp"


void CrowLogger::log(std::string message, crow::LogLevel crowLevel) {
    LogLevel customLevel = mapLogLevel(crowLevel);

    Log::getCoreLogger()->log(customLevel, message);
}

LogLevel CrowLogger::mapLogLevel(const crow::LogLevel crowLevel) {
    switch (crowLevel) {
        case crow::LogLevel::Debug: return LogLevel::Trace;
        case crow::LogLevel::Info: return LogLevel::Info;
        case crow::LogLevel::Warning: return LogLevel::Warn;
        case crow::LogLevel::Error: return LogLevel::Error;
        default: return LogLevel::Message;
    }
}