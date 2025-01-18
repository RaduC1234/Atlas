#pragma once

#include <chrono>

using namespace std::chrono;

// Do not split in cpp and hpp. It doesn't work, link error. The compiler needs the definitions.
class Time {
public:

    constexpr Time(milliseconds time = milliseconds(0)) : m_time(time) {}

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

    constexpr double toSeconds() const {
        return duration<double>(m_time).count();
    }

    constexpr double toMilliseconds() const {
        return duration<double, std::milli>(m_time).count();
    }

    constexpr long long toNanoseconds() const {
        return duration_cast<nanoseconds>(m_time).count();
    }

    constexpr bool operator<(const Time& rhs) const {
        return m_time < rhs.m_time;
    }

    constexpr bool operator>(const Time& rhs) const {
        return rhs < *this;
    }

    constexpr bool operator<=(const Time& rhs) const {
        return !(rhs < *this);
    }

    constexpr bool operator>=(const Time& rhs) const {
        return !(*this < rhs);
    }

    Time& operator=(const Time& rhs) {
        if (this != &rhs) {
            m_time = rhs.m_time;
        }
        return *this;
    }

    constexpr Time operator+(const Time& rhs) const {
        return Time(m_time + rhs.m_time);
    }

    constexpr std::chrono::milliseconds operator-(const Time& rhs) const {
        return m_time - rhs.m_time;
    }

    constexpr operator std::chrono::milliseconds() const {
        return m_time;
    }

private:
    std::chrono::milliseconds m_time;
};
