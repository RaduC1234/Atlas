#pragma once

#include "core/Core.hpp"

class FileSystem {
public:
    static void setWorkingDirectory(const std::string &programWorkingDirectory);

    static std::string fileToString(const std::string &filePath);
};


