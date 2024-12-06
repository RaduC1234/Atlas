#pragma once

#include <vector>
#include "Player.hpp"

struct LobbyMatchQuality {
    double overallQuality;           // Overall match quality (0-1)
    std::vector<double> matchups;    // Individual matchup qualities
    double skillSpread;             // Difference between highest and lowest skill
    size_t optimalTeamSize;         // Recommended team size based on ratings

    LobbyMatchQuality()
        : overallQuality(0.0)
        , skillSpread(0.0)
        , optimalTeamSize(2) {}
};

class LobbyValidator {
public:
    static bool isValidLobbySize(size_t size) {
        return size >= 2 && size <= 4;
    }

    static LobbyMatchQuality evaluateLobby(const std::vector<Player>& players) {
        LobbyMatchQuality quality;
        if (players.size() < 2 || players.size() > 4) {
            quality.overallQuality = 0.0;
            return quality;
        }

        double totalRating = 0.0;
        double minRating = players[0].getGlickoRating();
        double maxRating = players[0].getGlickoRating();

        // Calculate basic stats
        for (const auto& player : players) {
            totalRating += player.getGlickoRating();
            minRating = std::min(minRating, player.getGlickoRating());
            maxRating = std::max(maxRating, player.getGlickoRating());
        }

        quality.skillSpread = maxRating - minRating;
        double avgRating = totalRating / players.size();

        // Calculate all possible matchups
        for (size_t i = 0; i < players.size(); ++i) {
            for (size_t j = i + 1; j < players.size(); ++j) {
                auto matchQuality = MatchmakingManager::calculateMatchQuality(
                    players[i], players[j]);
                quality.matchups.push_back(matchQuality.quality);
            }
        }

        // Calculate overall quality
        double sumQuality = 0.0;
        for (double q : quality.matchups) {
            sumQuality += q;
        }
        quality.overallQuality = quality.matchups.empty() ? 0.0 : sumQuality / quality.matchups.size();

        // Determine optimal team size
        quality.optimalTeamSize = players.size() >= 4 ? 2 : players.size();

        return quality;
    }

    static bool canStartMatch(const std::vector<Player>& players) {
        if (!isValidLobbySize(players.size())) return false;
        
        auto quality = evaluateLobby(players);
        return quality.overallQuality >= 0.5; // Minimum quality threshold
    }
};