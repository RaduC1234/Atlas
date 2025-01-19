#pragma once
#include "Glicko2.hpp"
#include <vector>
#include <string>
#include <utility>
#include <cmath> // Ensure math functions are available
#include <algorithm>
struct MatchQualityResult {
    double quality; // Overall match quality (0-1)
    double skillSpread; // Rating spread between players
    bool isValid; // Whether the match is valid to start
    std::string reason; // Reason if match is invalid
};

class LobbyMatchQuality {
public:
    static constexpr size_t MIN_PLAYERS = 2;
    static constexpr size_t MAX_PLAYERS = 4;
    static constexpr double MIN_QUALITY = 0.6;
    static constexpr double MAX_RATING_SPREAD = 600.0;
    static constexpr double MAX_RD = 350.0;

    static MatchQualityResult evaluateMatch(const std::vector<Glicko2::Rating> &players) {
        MatchQualityResult result;
        if (!validatePlayerCount(players, result.reason)) {
            result.isValid = false;
            return result;
        }

        // Properly handle operations to avoid pointer misuse
        result.skillSpread = calculateRatingSpread(players);
        double avgRating = calculateAverageRating(players);

        if (!validateRatingSpread(players, result.reason)) {
            result.isValid = false;
            return result;
        }

        if (!validateRatingDeviations(players, result.reason)) {
            result.isValid = false;
            return result;
        }

        double accumulatedQuality = computeTotalQuality(players);
        double numComparisons = static_cast<double>(players.size() * (players.size() - 1)) / 2;
        result.quality = accumulatedQuality / numComparisons;

        result.isValid = true;
        return result;
    }

    static bool canPlayerJoin(const Glicko2::Rating &newPlayer,
                              const std::vector<Glicko2::Rating> &existingPlayers,
                              std::string &reason) {
        auto validRange = getValidRatingRange(existingPlayers);
        double minRating = validRange.first;
        double maxRating = validRange.second;

        if (newPlayer.rating < minRating || newPlayer.rating > maxRating) {
            reason = "Player rating is outside the valid range for this lobby.";
            return false;
        }
        return true;
    }

    static std::pair<double, double> getValidRatingRange(const std::vector<Glicko2::Rating> &existingPlayers) {
        double avgRating = calculateAverageRating(existingPlayers);
        double minRating = (avgRating - MAX_RATING_SPREAD / 2) > 0.0 ? (avgRating - MAX_RATING_SPREAD / 2) : 0.0;
        double maxRating = avgRating + MAX_RATING_SPREAD / 2;

        return std::make_pair(minRating, maxRating);
    }

private:
    static double calculateAverageRating(const std::vector<Glicko2::Rating> &players) {
        double sum = 0.0;
        for (const auto &player: players) {
            sum += player.rating;
        }
        return sum / players.size();
    }

    static double calculateRatingSpread(const std::vector<Glicko2::Rating> &players) {
        double avgRating = calculateAverageRating(players);
        double sumSquaredDiff = 0.0;
        for (const auto &player: players) {
            sumSquaredDiff += (player.rating - avgRating) * (player.rating - avgRating);
        }
        return std::sqrt(sumSquaredDiff / (players.size() - 1));
    }

    static double computeTotalQuality(const std::vector<Glicko2::Rating> &players) {
        double accumulatedQuality = 0.0;
        for (size_t i = 0; i < players.size(); ++i) {
            for (size_t j = i + 1; j < players.size(); ++j) {
                accumulatedQuality += calculatePairwiseQuality(players[i], players[j]);
            }
        }
        return accumulatedQuality;
    }

    static double calculatePairwiseQuality(const Glicko2::Rating &p1, const Glicko2::Rating &p2) {
        double ratingDiff = std::abs(p1.rating - p2.rating);
        double rdSum = p1.deviation + p2.deviation;
        return 1.0 / (1.0 + std::exp(3.0 * ratingDiff / rdSum));
    }

    static bool validatePlayerCount(const std::vector<Glicko2::Rating> &players, std::string &reason) {
        size_t playerCount = players.size();
        if (playerCount < MIN_PLAYERS || playerCount > MAX_PLAYERS) {
            reason = "Invalid number of players. Must be between " +
                     std::to_string(MIN_PLAYERS) + " and " +
                     std::to_string(MAX_PLAYERS) + ".";
            return false;
        }
        return true;
    }

    static bool validateRatingSpread(const std::vector<Glicko2::Rating> &players, std::string &reason) {
        double ratingSpread = calculateRatingSpread(players);
        if (ratingSpread > MAX_RATING_SPREAD) {
            reason = "Rating spread between players is too high. Maximum allowed is " +
                     std::to_string(MAX_RATING_SPREAD) + ".";
            return false;
        }
        return true;
    }

    static bool validateRatingDeviations(const std::vector<Glicko2::Rating> &players, std::string &reason) {
        for (const auto &player: players) {
            if (player.deviation > MAX_RD) {
                reason = "One or more players have a rating deviation that is too high. "
                         "Maximum allowed is " + std::to_string(MAX_RD) + ".";
                return false;
            }
        }
        return true;
    }
};
