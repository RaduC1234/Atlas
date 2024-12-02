#pragma once

#include <Atlas.hpp>
#include <Windows.h>
#include "Player.hpp"
#include "Match.hpp"

// Structure pentru calitatea meciului (trebuie să fie identică cu cea din DLL)
struct MatchQuality {
    double quality;          // 0-1 score indicating match fairness
    double expectedOutcome; // Probability of player 1 winning
};

constexpr auto dllName = "matchmaking.dll";

class MatchmakingManager {
public:
    static void init() {
#ifdef WIN32
        hDLL = LoadLibrary(dllName);
        if (!hDLL) {
            AT_FATAL("Failed to load {0}", dllName);
            return;
        }

        // Core functions
        getDefaultValuesFunc = GetProcAddress(hDLL, "getDefaultValues");
        updateRatingsFunc = GetProcAddress(hDLL, "updateRatings");
        convertToGlicko2ScaleFunc = GetProcAddress(hDLL, "convertToGlicko2Scale");
        convertFromGlicko2ScaleFunc = GetProcAddress(hDLL, "convertFromGlicko2Scale");

        // Additional functions
        calculateMatchQualityFunc = GetProcAddress(hDLL, "calculateMatchQuality");
        validateAndAdjustRatingsFunc = GetProcAddress(hDLL, "validateAndAdjustRatings");
        calculateRatingConfidenceFunc = GetProcAddress(hDLL, "calculateRatingConfidence");
        calculateRatingChangePreviewFunc = GetProcAddress(hDLL, "calculateRatingChangePreview");

        if (!getDefaultValuesFunc || !updateRatingsFunc ||
            !convertToGlicko2ScaleFunc || !convertFromGlicko2ScaleFunc) {
            shutdown();
            AT_ERROR("Failed to load core functions from {0}", dllName);
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

    // Existing functions remain the same
    static void getDefaultValues(double &rating, double &rd, double &volatility) {
#ifdef WIN32
        typedef void (*FuncType)(double*, double*, double*);
        FuncType func = reinterpret_cast<FuncType>(getDefaultValuesFunc);
        func(&rating, &rd, &volatility);
#endif
    }

    static void updateMatchRatings(Match& match) {
#ifdef WIN32
        typedef void (*UpdateFunc)(double*, double*, double*,
                                 const double*, const double*, const double*, int);

        UpdateFunc updateFunc = reinterpret_cast<UpdateFunc>(updateRatingsFunc);

        const auto& players = match.GetPlayers();
        const auto& winner = match.GetWinner();

        for (auto& player : players) {
            std::vector<double> oppRatings;
            std::vector<double> oppRDs;
            std::vector<double> scores;

            for (const auto& opponent : players) {
                if (opponent.getUsername() != player.getUsername()) {
                    oppRatings.push_back(opponent.getGlickoRating());
                    oppRDs.push_back(opponent.getRatingDeviation());

                    if (winner.getUsername() == player.getUsername())
                        scores.push_back(1.0);
                    else if (winner.getUsername() == opponent.getUsername())
                        scores.push_back(0.0);
                    else
                        scores.push_back(0.5);
                }
            }

            double newRating = player.getGlickoRating();
            double newRD = player.getRatingDeviation();
            double newVol = player.getVolatility();

            updateFunc(&newRating, &newRD, &newVol,
                      oppRatings.data(), oppRDs.data(), scores.data(),
                      static_cast<int>(oppRatings.size()));

            player.setGlickoRating(newRating)
                 .setRatingDeviation(newRD)
                 .setVolatility(newVol);
        }
#endif
    }

    static double convertToGlicko2Scale(double rating) {
#ifdef WIN32
        typedef double (*ConvertFunc)(double);
        ConvertFunc func = reinterpret_cast<ConvertFunc>(convertToGlicko2ScaleFunc);
        return func(rating);
#else
        return rating;
#endif
    }

    static double convertFromGlicko2Scale(double rating) {
#ifdef WIN32
        typedef double (*ConvertFunc)(double);
        ConvertFunc func = reinterpret_cast<ConvertFunc>(convertFromGlicko2ScaleFunc);
        return func(rating);
#else
        return rating;
#endif
    }

    // New utility functions
    static MatchQuality calculateMatchQuality(const Player& p1, const Player& p2) {
#ifdef WIN32
        typedef MatchQuality (*QualityFunc)(double, double, double, double);
        QualityFunc func = reinterpret_cast<QualityFunc>(calculateMatchQualityFunc);
        return func(p1.getGlickoRating(), p1.getRatingDeviation(),
                   p2.getGlickoRating(), p2.getRatingDeviation());
#else
        return MatchQuality{0.0, 0.5};
#endif
    }

    static void validateAndAdjustRatings(Player& player) {
#ifdef WIN32
        typedef void (*ValidateFunc)(double*, double*);
        ValidateFunc func = reinterpret_cast<ValidateFunc>(validateAndAdjustRatingsFunc);
        double rating = player.getGlickoRating();
        double rd = player.getRatingDeviation();
        func(&rating, &rd);
        player.setGlickoRating(rating).setRatingDeviation(rd);
#endif
    }

    static double calculateRatingConfidence(const Player& player) {
#ifdef WIN32
        typedef double (*ConfidenceFunc)(double);
        ConfidenceFunc func = reinterpret_cast<ConfidenceFunc>(calculateRatingConfidenceFunc);
        return func(player.getRatingDeviation());
#else
        return 0.5;
#endif
    }

    static void calculateRatingChangePreview(const Player& player, const Player& opponent, double score,
                                           double& expectedRatingChange, double& expectedRdChange) {
#ifdef WIN32
        typedef void (*PreviewFunc)(double, double, double, double, double, double, double*, double*);
        PreviewFunc func = reinterpret_cast<PreviewFunc>(calculateRatingChangePreviewFunc);
        func(player.getGlickoRating(), player.getRatingDeviation(), player.getVolatility(),
             opponent.getGlickoRating(), opponent.getRatingDeviation(), score,
             &expectedRatingChange, &expectedRdChange);
#endif
    }

private:
#ifdef WIN32
    static HMODULE hDLL;
    // Core function pointers
    static FARPROC getDefaultValuesFunc;
    static FARPROC updateRatingsFunc;
    static FARPROC convertToGlicko2ScaleFunc;
    static FARPROC convertFromGlicko2ScaleFunc;
    // Additional function pointers
    static FARPROC calculateMatchQualityFunc;
    static FARPROC validateAndAdjustRatingsFunc;
    static FARPROC calculateRatingConfidenceFunc;
    static FARPROC calculateRatingChangePreviewFunc;
#endif
};

#ifdef WIN32
HMODULE MatchmakingManager::hDLL = nullptr;
// Core functions
FARPROC MatchmakingManager::getDefaultValuesFunc = nullptr;
FARPROC MatchmakingManager::updateRatingsFunc = nullptr;
FARPROC MatchmakingManager::convertToGlicko2ScaleFunc = nullptr;
FARPROC MatchmakingManager::convertFromGlicko2ScaleFunc = nullptr;
// Additional functions
FARPROC MatchmakingManager::calculateMatchQualityFunc = nullptr;
FARPROC MatchmakingManager::validateAndAdjustRatingsFunc = nullptr;
FARPROC MatchmakingManager::calculateRatingConfidenceFunc = nullptr;
FARPROC MatchmakingManager::calculateRatingChangePreviewFunc = nullptr;
#endif