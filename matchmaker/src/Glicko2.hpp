#pragma once
#include <vector>
#include <cmath>
class Glicko2 {
public:
    struct Rating {
        double rating;       // Current rating (default 1500)
        double deviation;    // Rating deviation (default 350)
        double volatility;   // Rating volatility (default 0.06)
        int wins = 0;
        int losses = 0;

        Rating() : rating(1500.0), deviation(350.0), volatility(0.06) {}
        Rating(double r, double d, double v) : rating(r), deviation(d), volatility(v) {}

        // Get confidence interval bounds
        [[nodiscard]] double getLowerBound() const { return rating - 2 * deviation; }
        [[nodiscard]] double getUpperBound() const { return rating + 2 * deviation; }

        // Get rating range for matchmaking
        [[nodiscard]] double getMinMatchRating() const { return rating - 3 * deviation; }
        [[nodiscard]] double getMaxMatchRating() const { return rating + 3 * deviation; }
    };

    struct MatchResult {
        Rating opponent;
        double score{};  // 1.0 for win, 0.5 for draw, 0.0 for loss
    };

    // Core functions
    static Rating getDefaultRating() { return {}; }
    static Rating updateRating(const Rating& oldRating, const std::vector<MatchResult>& results);

    // Scale conversions
    static double toGlicko2Scale(double rating) { return (rating - 1500.0) / 173.7178; }
    static double fromGlicko2Scale(double rating) { return (rating * 173.7178) + 1500.0; }

    // Match quality assessment
    static double calculateMatchQuality(const Rating& player1, const Rating& player2);
    static double calculateExpectedScore(const Rating& player1, const Rating& player2);

private:
    static constexpr double TAU = 0.5;         // System constant
    static constexpr double EPSILON = 0.000001; // Convergence tolerance
    static constexpr int MAX_ITERATIONS = 100;  // Maximum iterations

    static double computeVariance(const std::vector<MatchResult>& results, const Rating& player);
    static double computeDelta(const std::vector<MatchResult>& results, const Rating& player, double variance);
    static double updateVolatility(const Rating& rating, double delta, double variance);
};