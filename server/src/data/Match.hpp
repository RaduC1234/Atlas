//
// Created by Karina on 11/15/2024.
//
#pragma once
#include <glm/ext/scalar_uint_sized.hpp>

#include "Atlas.hpp"
#include "Player.hpp"

class Match {
public:
    Match() : m_id(0), m_timestamp(static_cast<uint32_t>(std::time(nullptr))), m_glickoMatchWeight(1.0) {}

    explicit Match(uint32_t timestamp, std::vector<int> playerIds, const int winnerId, double glickoMatchWeight = 1.0)
        : m_timestamp(timestamp), m_playerIds(std::move(playerIds)),
          m_winnerId(winnerId), m_glickoMatchWeight(glickoMatchWeight) {}

    ~Match() = default;

    int getId() const { return m_id; }
    uint32_t getTimestamp() const { return m_timestamp; }
    const std::vector<int>& getPlayerIds() const { return m_playerIds; }
    int getWinnerId() const { return m_winnerId; }
    double getGlickoMatchWeight() const { return m_glickoMatchWeight; }

    void setId(int id) { m_id = id; }
    void setTimestamp(uint32_t timestamp) { m_timestamp = timestamp; }
    void setPlayerIds(const std::vector<int>& playerIds) { m_playerIds = playerIds; }
    void setWinnerId(int winnerId) { m_winnerId = winnerId; }
    void setGlickoMatchWeight(double glickoMatchWeight) { m_glickoMatchWeight = glickoMatchWeight; }

private:
    int m_id;
    uint32_t m_timestamp;
    std::vector<int> m_playerIds;
    int m_winnerId;
    double m_glickoMatchWeight;
};
