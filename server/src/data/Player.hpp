#pragma once

#include "Atlas.hpp"

class Player {
public:
    Player() = default;

    explicit Player(int id, std::string username, std::string password, uint32_t mmr,
        std::string profilePicture, int matchId = -1, double glickoRating = 1500.0, double ratingDeviation = 350.0, double volatility = 0.06)
        : m_id(id), m_username(std::move(username)), m_password(std::move(password)),
        m_mmr(mmr), m_profilePicture(std::move(profilePicture)),
        m_matchId(matchId), m_glickoRating(glickoRating), m_ratingDeviation(ratingDeviation), m_volatility(volatility) {}

    ~Player() = default;

    int getId() const { return m_id; }

    const std::string& getUsername() const { return m_username; }
    const std::string& getPassword() const { return m_password; }
    uint32_t getMmr() const { return m_mmr; }
    const std::string& getProfilePicture() const { return m_profilePicture; }
    double getGlickoRating() const { return m_glickoRating; }
    double getRatingDeviation() const { return m_ratingDeviation; }
    double getVolatility() const { return m_volatility; }
    int getMatchId() const { return m_matchId; }

    void setId(int id) { m_id = id; }
    void setUsername(const std::string& username) { m_username = username; }
    void setPassword(const std::string& password) { m_password = password; }
    void setMmr(uint32_t mmr) { m_mmr = mmr; }
    void setProfilePicture(const std::string& profilePicture) { m_profilePicture = profilePicture; }
    void setGlickoRating(double glickoRating) { m_glickoRating = glickoRating; }
    void setRatingDeviation(double ratingDeviation) { m_ratingDeviation = ratingDeviation; }
    void setVolatility(double volatility) { m_volatility = volatility; }
    void setMatchId(int matchId) { m_matchId = matchId; }

private:
    int m_id;
    std::string m_username;
    std::string m_password;
    uint32_t m_mmr;
    std::string m_profilePicture;
    int m_matchId; // Foreign Key referencing the match this player is part of.
    double m_glickoRating;
    double m_ratingDeviation;
    double m_volatility;
};