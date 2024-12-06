#pragma once

#include "Atlas.hpp"

class Player {
public:
    Player() : m_glickoRating(1500.0), m_ratingDeviation(350.0), m_volatility(0.06) {};

    Player(const std::string &username, const std::string &password, uint32_t mmr,
        uint32_t profilePicture, double glickoRating = 1500.0, double ratingDeviation = 350.0, double volatility = 0.06) :
        m_username(username), m_password(password), m_mmr(mmr), m_profilePicture(profilePicture),
        m_glickoRating(glickoRating), m_ratingDeviation(ratingDeviation), m_volatility(volatility) {};

    ~Player() = default;

    const std::string &getUsername() const {
        return m_username;
    }

    const std::string &getPassword() const {
        return m_password;
    }

    uint32_t getMmr() const {
        return m_mmr;
    }

    uint32_t getProfilePicture() const {
        return m_profilePicture;
    }

    double getGlickoRating() const
    {
        return m_glickoRating;
    }

    double getRatingDeviation() const
    {
        return m_ratingDeviation;
    }

    double getVolatility() const
    {
        return m_volatility;
    }

    Player setUsername(const std::string &username) {
        this->m_username = username;
        return *this;
    }

    Player setPassword(const std::string &password) {
        this->m_password = password;
        return *this;
    }

    Player setMmr(uint32_t mmr) {
        this->m_mmr = mmr;
        return *this;
    }

    Player setProfilePicture(uint32_t profilePicture) {
        this->m_profilePicture = profilePicture;
        return *this;
    }

    Player setGlickoRating(double glickoRating)
    {
        this->m_glickoRating = glickoRating;
    }

    Player setRatingDeviation(double ratingDeviation)
    {
        this->m_ratingDeviation = ratingDeviation;
    }

    Player setVolatility(double volatility)
    {
        this->m_volatility = volatility;
    }

private:
    std::string m_username; // primary key
    std::string m_password;
    uint32_t m_mmr;
    uint32_t m_profilePicture;
    double m_glickoRating;
    double m_ratingDeviation;
    double m_volatility;
};