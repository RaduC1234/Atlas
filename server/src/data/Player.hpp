#pragma once

#include "Atlas.hpp"

class Player {
public:
    Player();

    Player(const std::string &username, const std::string &password, uint32_t mmr, uint32_t profilePicture) : m_username(username), m_password(password), m_mmr(mmr), m_profilePicture(profilePicture) {}

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

private:
    std::string m_username; // primary key
    std::string m_password;
    uint32_t m_mmr;
    uint32_t m_profilePicture;
};
