#include "Matchmaking.hpp"
#include <Windows.h>
#include <cstring>
#include <algorithm>
#include <cmath>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

extern "C" {
    MATCHMAKING_API void getDefaultRating(Rating* rating) {
        if (rating) {
            rating->rating = 1500.0;
            rating->deviation = 350.0;
            rating->volatility = 0.06;
            rating->wins = 0;
            rating->losses = 0;
        }
    }

    MATCHMAKING_API void evaluateMatch(const Rating* players, int numPlayers, MatchQuality* result) {
        if (!players || !result || numPlayers < 2 || numPlayers > 4) {
            result->isValid = false;
            strcpy_s(result->reason, "Invalid parameters or player count");
            return;
        }

        double avgRating = 0;
        for (int i = 0; i < numPlayers; i++) {
            avgRating += players[i].rating;
        }
        avgRating /= numPlayers;

        double maxSpread = 0;
        for (int i = 0; i < numPlayers; i++) {
            if (std::abs(players[i].rating - avgRating) > maxSpread) {
                maxSpread = std::abs(players[i].rating - avgRating);
            }
        }

        result->skillSpread = maxSpread;
        result->quality = 1.0 - (maxSpread / 800.0);
        result->isValid = maxSpread < 600.0;

        if (!result->isValid) {
            strcpy_s(result->reason, "Skill spread too high");
        }
    }

    MATCHMAKING_API bool canPlayerJoin(const Rating* newPlayer,
                                       const Rating* existingPlayers,
                                       int numExistingPlayers,
                                       char* reason,
                                       int reasonSize) {
        if (!newPlayer || !existingPlayers || numExistingPlayers < 1) {
            strncpy_s(reason, reasonSize, "Invalid parameters", _TRUNCATE);
            return false;
        }

        if (numExistingPlayers >= 4) {
            strncpy_s(reason, reasonSize, "Lobby is full", _TRUNCATE);
            return false;
        }

        double avgRating = 0;
        for (int i = 0; i < numExistingPlayers; i++) {
            avgRating += existingPlayers[i].rating;
        }
        avgRating /= numExistingPlayers;

        double ratingDiff = std::abs(newPlayer->rating - avgRating);
        if (ratingDiff > 600.0) {
            strncpy_s(reason, reasonSize, "Rating difference too high", _TRUNCATE);
            return false;
        }

        return true;
    }

    MATCHMAKING_API void getValidRatingRange(const Rating* existingPlayers,
                                             int numExistingPlayers,
                                             double* minRating,
                                             double* maxRating,
                                             bool* success,
                                             char* reason,
                                             int reasonSize) {
        if (!existingPlayers || !minRating || !maxRating || numExistingPlayers < 1) {
            *success = false;
            strncpy_s(reason, reasonSize, "Invalid parameters", _TRUNCATE);
            return;
        }

        double avgRating = 0;
        for (int i = 0; i < numExistingPlayers; i++) {
            avgRating += existingPlayers[i].rating;
        }
        avgRating /= numExistingPlayers;

        *minRating = avgRating - 600.0;
        *maxRating = avgRating + 600.0;
        *success = true;
    }

    MATCHMAKING_API void updateRatings(Rating* currentRating,
                                       const Rating* opponents,
                                       const double* results,
                                       int numOpponents,
                                       bool* success,
                                       char* errorMsg,
                                       int errorMsgSize) {
        if (!currentRating || !opponents || !results || numOpponents < 1) {
            *success = false;
            strncpy_s(errorMsg, errorMsgSize, "Invalid parameters", _TRUNCATE);
            return;
        }

        double ratingChange = 0;
        for (int i = 0; i < numOpponents; i++) {
            double expectedScore = 1.0 / (1.0 + pow(10.0, (opponents[i].rating - currentRating->rating) / 400.0));
            ratingChange += 32.0 * (results[i] - expectedScore);
        }

        currentRating->rating += ratingChange;

        for (int i = 0; i < numOpponents; i++) {
            if (results[i] > 0.6) currentRating->wins++;
            else if (results[i] < 0.4) currentRating->losses++;
        }

        *success = true;
    }
}
