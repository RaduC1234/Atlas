#pragma once

#include <Atlas.hpp>
#include <Windows.h>
#include "../data/DatabaseManager.hpp"
#include "../data/Player.hpp"

class MatchmakingManager {
public:
    // DLL structs matching the binary interface
    struct Rating {
        double rating;
        double deviation;
        double volatility;
        int wins;
        int losses;
    };

    struct MatchQuality {
        bool isValid;
        char reason[256];
        double skillSpread;
        double quality;
    };

    static void init() {
#ifdef WIN32
        hDLL = LoadLibrary("matchmaker.dll");
        if (!hDLL) {
            AT_FATAL("Failed to load matchmaker.dll");
            return;
        }

        // Load core functions
        getDefaultRatingFunc = (GetDefaultRatingFunc)GetProcAddress(hDLL, "getDefaultRating");
        evaluateMatchFunc = (EvaluateMatchFunc)GetProcAddress(hDLL, "evaluateMatch");
        canPlayerJoinFunc = (CanPlayerJoinFunc)GetProcAddress(hDLL, "canPlayerJoin");
        getValidRatingRangeFunc = (GetValidRatingRangeFunc)GetProcAddress(hDLL, "getValidRatingRange");
        updateRatingsFunc = (UpdateRatingsFunc)GetProcAddress(hDLL, "updateRatings");

        if (!getDefaultRatingFunc || !evaluateMatchFunc || !canPlayerJoinFunc ||
            !getValidRatingRangeFunc || !updateRatingsFunc) {
            shutdown();
            AT_ERROR("Failed to load core functions from matchmaker.dll");
        }
#endif
    }

    static void shutdown() {
#ifdef WIN32
        if (hDLL) {
            FreeLibrary(hDLL);
            hDLL = nullptr;
        }
#endif
    }

    static void getDefaultRating(Player& player) {
#ifdef WIN32
        Rating rating;
        getDefaultRatingFunc(&rating);

        player.setMmr(1500); // Default MMR
        player.setGlickoRating(rating.rating);
        player.setRatingDeviation(rating.deviation);
        player.setVolatility(rating.volatility);
#endif
    }

    static MatchQuality evaluateMatch(const std::vector<Player>& players) {
#ifdef WIN32
        std::vector<Rating> ratings;
        for (const auto& player : players) {
            ratings.push_back({
                player.getGlickoRating(),
                player.getRatingDeviation(),
                player.getVolatility(),
                0,  // wins - not needed for evaluation
                0   // losses - not needed for evaluation
            });
        }

        MatchQuality quality;
        evaluateMatchFunc(ratings.data(), static_cast<int>(ratings.size()), &quality);
        return quality;
#else
        MatchQuality quality = {};
        quality.isValid = false;
        strcpy_s(quality.reason, "DLL not loaded");
        return quality;
#endif
    }

    static bool canPlayerJoin(const Player& newPlayer, const std::vector<Player>& existingPlayers, std::string& reason) {
#ifdef WIN32
        Rating newRating{
            newPlayer.getGlickoRating(),
            newPlayer.getRatingDeviation(),
            newPlayer.getVolatility(),
            0, 0  // wins/losses not needed for this check
        };

        std::vector<Rating> existingRatings;
        for (const auto& player : existingPlayers) {
            existingRatings.push_back({
                player.getGlickoRating(),
                player.getRatingDeviation(),
                player.getVolatility(),
                0, 0
            });
        }

        char reasonBuffer[256];
        bool canJoin = canPlayerJoinFunc(&newRating, existingRatings.data(),
                                       static_cast<int>(existingRatings.size()),
                                       reasonBuffer, sizeof(reasonBuffer));
        reason = reasonBuffer;
        return canJoin;
#else
        reason = "DLL not loaded";
        return false;
#endif
    }

    static void updateRatings(const std::vector<Player>& players, const Player& winner) {
#ifdef WIN32
        for (const auto& currentPlayer : players) {
            std::vector<Rating> opponentRatings;
            std::vector<double> results;

            for (const auto& opponent : players) {
                if (opponent.getId() != currentPlayer.getId()) {
                    opponentRatings.push_back({
                        opponent.getGlickoRating(),
                        opponent.getRatingDeviation(),
                        opponent.getVolatility(),
                        0, 0
                    });

                    if (currentPlayer.getId() == winner.getId()) {
                        results.push_back(1.0);
                    } else if (opponent.getId() == winner.getId()) {
                        results.push_back(0.0);
                    } else {
                        results.push_back(0.5);
                    }
                }
            }

            Rating currentRating{
                currentPlayer.getGlickoRating(),
                currentPlayer.getRatingDeviation(),
                currentPlayer.getVolatility(),
                0, 0
            };

            bool success;
            char errorMsg[256];

            updateRatingsFunc(&currentRating, opponentRatings.data(), results.data(),
                            static_cast<int>(opponentRatings.size()),
                            &success, errorMsg, sizeof(errorMsg));

            if (success) {
                Player updatedPlayer = currentPlayer;
                updatedPlayer.setGlickoRating(currentRating.rating);
                updatedPlayer.setRatingDeviation(currentRating.deviation);
                updatedPlayer.setVolatility(currentRating.volatility);
                DatabaseManager::update(updatedPlayer);
            } else {
                AT_ERROR("Failed to update ratings for player {0}: {1}",
                        currentPlayer.getUsername(), errorMsg);
            }
        }
#endif
    }

private:
#ifdef WIN32
    // Function pointer types
    typedef void (*GetDefaultRatingFunc)(Rating*);
    typedef void (*EvaluateMatchFunc)(const Rating*, int, MatchQuality*);
    typedef bool (*CanPlayerJoinFunc)(const Rating*, const Rating*, int, char*, int);
    typedef void (*GetValidRatingRangeFunc)(const Rating*, int, double*, double*, bool*, char*, int);
    typedef void (*UpdateRatingsFunc)(Rating*, const Rating*, const double*, int, bool*, char*, int);

    inline static HMODULE hDLL = nullptr;
    inline static GetDefaultRatingFunc getDefaultRatingFunc = nullptr;
    inline static EvaluateMatchFunc evaluateMatchFunc = nullptr;
    inline static CanPlayerJoinFunc canPlayerJoinFunc = nullptr;
    inline static GetValidRatingRangeFunc getValidRatingRangeFunc = nullptr;
    inline static UpdateRatingsFunc updateRatingsFunc = nullptr;
#endif
};