#pragma once

#include <Atlas.hpp>
#include <crow.h>

/**
 * Proxy class to pass all the native Crow logger messages to Atlas one
 */
class CrowLogger : public crow::ILogHandler {
public:
    void log(std::string message, crow::LogLevel crowLevel) override;

private:
    static LogLevel mapLogLevel(crow::LogLevel crowLevel);
};
