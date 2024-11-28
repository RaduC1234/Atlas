#pragma once

#include "core/Core.hpp"

using namespace std::chrono;

class Time {
public:
    Time(milliseconds time = milliseconds(0)) : m_time(time) {}

    static Time now();

    Time& addSeconds(double seconds);

    Time& addMilliseconds(double milliseconds);

    Time& addNanoseconds(double nanoseconds);

    double toSeconds() const;

    double toMilliseconds() const;

    long long toNanoseconds() const;

    bool operator<(const Time& rhs) const;

    bool operator>(const Time& rhs) const;

    bool operator<=(const Time& rhs) const;

    bool operator>=(const Time& rhs) const;

    Time& operator=(const Time& rhs);

    Time operator+(const Time& rhs) const;

    std::chrono::milliseconds operator-(const Time& rhs) const;

    operator std::chrono::milliseconds() const;

private:
    std::chrono::milliseconds m_time;  // Encapsulated std::chrono::milliseconds
};


class FileSystem {
public:
    static void setWorkingDirectory(const std::string &programWorkingDirectory);

    static std::string fileToString(const std::string &filePath);
};

class FileDialogs {
public:

    static std::string openFile(const char *filter);

    static std::string saveFile(const char *filter);
};
