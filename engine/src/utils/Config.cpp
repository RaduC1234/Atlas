#include "Config.hpp"


ConfigValue::ConfigValue(const std::string &value) : value(value) {}

std::string ConfigValue::toString(const std::string &defaultValue) const {
    return !value.empty() ? value : defaultValue;
}

bool ConfigValue::toBool(bool defaultValue) const {
    if (value == "true" || value == "1" || value == "yes") return true;
    if (value == "false" || value == "0" || value == "no") return false;
    return defaultValue;
}

int ConfigValue::toInt(int defaultValue) const {
    try {
        return !value.empty() ? std::stoi(value) : defaultValue;
    } catch (...) {
        return defaultValue;
    }
}

double ConfigValue::toDouble(double defaultValue) const {
    try {
        return !value.empty() ? std::stod(value) : defaultValue;
    } catch (...) {
        return defaultValue;
    }
}

float ConfigValue::toFloat(float defaultValue) const {
    try {
        return !value.empty() ? std::stof(value) : defaultValue;
    } catch (...) {
        return defaultValue;
    }
}

ConfigValue Config::operator[](const std::string &key) const {
    auto it = data.find(key);
    return ConfigValue(it != data.end() ? it->second : "");
}

ConfigParser::ConfigParser(const std::string &filename) : filename(filename) {}

Config ConfigParser::build() {
    Config config;
    std::ifstream file(filename);

    if (!file) {
        std::cerr << "Could not open config file: " << filename << std::endl;
        return config;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Remove comments and trim whitespace
        line = line.substr(0, line.find('#'));
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty() || line.find('=') == std::string::npos) {
            continue;
        }

        std::istringstream lineStream(line);
        std::string key, value;
        if (std::getline(lineStream, key, '=') && std::getline(lineStream, value)) {
            // Trim whitespace from key and value
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            config.data[key] = value;
        }
    }

    return config;
}
