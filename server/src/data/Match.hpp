//
// Created by Karina on 11/15/2024.
//
#pragma once
#include <glm/ext/scalar_uint_sized.hpp>

#include "Atlas.hpp"
#include "Player.hpp"

class Match
{
public:
    Match(): m_id(0), m_timestamp(static_cast<uint32_t>(std::time(nullptr))), m_glickoMatchWeight(1.0) {}

    Match(uint32_t id, uint32_t timestamp, const std::vector<Player>& players,
        const Player& winner, double glickoMatchWeight = 1.0): m_id(id), m_timestamp(timestamp),
        m_players(players), m_winner(winner), m_glickoMatchWeight(glickoMatchWeight) {}

    ~Match() = default;

    uint32_t GetId() const
    {
        return m_id;
    }

    uint32_t GetTimestamp() const
    {
        return m_timestamp;
    }

    const std::vector<Player>& GetPlayers() const
    {
        return m_players;
    }

    const Player& GetWinner() const
    {
        return m_winner;
    }

    double GetGlickoMatchWeight() const
    {
        return m_glickoMatchWeight;
    }

    Match& setId (uint32_t id)
    {
        m_id = id;
        return *this;
    }

    Match& setTimestamp(uint32_t timestamp)
    {
        m_timestamp = timestamp;
        return *this;
    }

    Match& setPlayers(const std::vector<Player>& players) {
        m_players = players;
        return *this;
    }

    Match& setWinner(const Player& winner)
    {
        m_winner = winner;
        return *this;
    }

    Match& setGlickoMatchWeight(double glickoMatchWeight)
    {
        m_glickoMatchWeight = glickoMatchWeight;
        return *this;
    }

private:
    uint32_t m_id;
    uint32_t m_timestamp;
    std::vector<Player> m_players;
    Player m_winner;
    double m_glickoMatchWeight;
};
