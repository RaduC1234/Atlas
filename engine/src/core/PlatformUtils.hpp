#pragma once

#include "core/Core.hpp"
#include "core/Window.hpp"

class Time {
public:

    static double getTime();
};

class FileSystem {
public:
    static void setWorkingDirectory(std::string programWorkingDirectory);
};

class FileDialogs {
private:
    static Window *m_window;
public:

    static void init(const Window &window);

    static std::string openFile(const char *filter);
    static std::string saveFile(const char* filter);
};
