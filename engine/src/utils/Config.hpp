#pragma once

#include "core/Core.hpp"

class ConfigValue {
public:
    explicit ConfigValue(const std::string& value);

    std::string toString(const std::string& defaultValue = "") const;

    bool toBool(bool defaultValue = false) const;

    int toInt(int defaultValue = 0) const;

    double toDouble(double defaultValue = 0.0) const;

    float toFloat(float defaultValue = 0.0f) const;

private:
    std::string value;
};

class Config {
public:
    ConfigValue operator[](const std::string& key) const;

    static Config build(const std::string& filePath);

private:
    std::unordered_map<std::string, std::string> data;
};

