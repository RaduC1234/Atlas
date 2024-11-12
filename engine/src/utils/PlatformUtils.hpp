#pragma once

#include "core/Core.hpp"

using namespace std::chrono;

class Time {
public:
    Time(milliseconds time = milliseconds(0)) : m_time(time) {}

    static Time now() {
        auto now = high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return {duration_cast<milliseconds>(duration)};
    }

    Time& addSeconds(double seconds) {
        m_time += duration_cast<milliseconds>(duration<double>(seconds));
        return *this;
    }

    Time& addMilliseconds(double milliseconds) {
        m_time += std::chrono::milliseconds(static_cast<long long>(milliseconds));
        return *this;
    }

    Time& addNanoseconds(double nanoseconds) {
        m_time += duration_cast<milliseconds>(duration<long long, std::nano>(static_cast<long long>(nanoseconds)));
        return *this;
    }

    double toSeconds() const {
        return duration<double>(m_time).count();
    }

    double toMilliseconds() const {
        return duration<double, std::milli>(m_time).count();
    }

    long long toNanoseconds() const {
        return duration_cast<nanoseconds>(m_time).count();
    }

    bool operator<(const Time& rhs) const {
        return m_time < rhs.m_time;
    }

    bool operator>(const Time& rhs) const {
        return rhs < *this;
    }

    bool operator<=(const Time& rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const Time& rhs) const {
        return !(*this < rhs);
    }

    Time& operator=(const Time& rhs) {
        if (this != &rhs) {
            m_time = rhs.m_time;
        }
        return *this;
    }

    Time operator+(const Time& rhs) const {
        return Time(m_time + rhs.m_time);
    }

    std::chrono::milliseconds operator-(const Time& rhs) const {
        return m_time - rhs.m_time;
    }

    operator std::chrono::milliseconds() const {
        return m_time;
    }

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
