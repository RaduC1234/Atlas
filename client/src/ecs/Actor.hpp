#pragma once

#include <cstdint>//Add to core.hpp

class Actor {
public:
    using IdType = std::uint32_t;

    Actor() = default;
    explicit Actor(IdType id) : id(id) {}

    [[nodiscard]] IdType getId() const { return id; }
    [[nodiscard]] bool isValid() const { return id != 0; }

    bool operator==(const Actor& other) const { return id == other.id; }
    bool operator!=(const Actor& other) const { return id != other.id; }

private:
    IdType id = 0;
};
