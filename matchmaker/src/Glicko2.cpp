#include "Glicko2.hpp"
#include <vector>
#include <cmath>
namespace {
    constexpr double PI = 3.14159265358979323846;
    constexpr double GLICKO2_CONVERSION_FACTOR = 173.7178;
    constexpr double RATING_DIFFERENCE_SCALE = 800.0; // ~4 standard deviations
    constexpr double AVERAGE_DEVIATION_SCALE = 350.0;

    double g(double deviation) {
        return 1.0 / std::sqrt(1.0 + (3.0 * deviation * deviation) / (PI * PI));
    }

    double E(double playerRating, double opponentRating, double opponentDeviation) {
        return 1.0 / (1.0 + std::exp(-g(opponentDeviation) * (playerRating - opponentRating)));
    }

    double convertToGlicko2Deviation(double deviation) {
        return deviation / GLICKO2_CONVERSION_FACTOR;
    }
}

Glicko2::Rating Glicko2::updateRating(const Rating& oldRating,
                                     const std::vector<MatchResult>& results) {
    if (results.empty()) {
        return oldRating;
    }
    // Convert rating to Glicko-2 scale
    double mu = toGlicko2Scale(oldRating.rating);
    double phi = convertToGlicko2Deviation(oldRating.deviation);
    double sigma = oldRating.volatility;

    // Step 1: Calculate v (estimated variance)
    double v = computeVariance(results, oldRating);

    // Step 2: Calculate delta (estimated improvement)
    double delta = computeDelta(results, oldRating, v);

    // Step 3: Update volatility
    double newSigma = updateVolatility(oldRating, delta, v);

    // Step 4: Update rating deviation
    double phiStar = std::sqrt(phi * phi + newSigma * newSigma);
    double newPhi = 1.0 / std::sqrt(1.0 / (phiStar * phiStar) + 1.0 / v);

    // Step 5: Update rating
    double newMu = mu;
    if (v > 0) {
        double sum = 0.0;
        for (const auto& result : results) {
            double oppMu = toGlicko2Scale(result.opponent.rating);
            double oppPhi = convertToGlicko2Deviation(result.opponent.deviation);
            sum += g(oppPhi) * (result.score - E(mu, oppMu, oppPhi));
        }
        newMu = mu + (newPhi * newPhi) * sum;
    }

    // Convert back to original scale
    Rating newRating;
    newRating.rating = fromGlicko2Scale(newMu);
    newRating.deviation = newPhi * GLICKO2_CONVERSION_FACTOR;
    newRating.volatility = newSigma;

    // Update win/loss record
    newRating.wins = oldRating.wins;
    newRating.losses = oldRating.losses;
    for (const auto& result : results) {
        if (result.score > 0.6) newRating.wins++;
        else if (result.score < 0.4) newRating.losses++;
    }
    return newRating;
}

double Glicko2::calculateMatchQuality(const Rating& player1, const Rating& player2) {
    // Calculate match quality based on:
    // 1. Rating difference
    // 2. Rating deviations (uncertainty)
    // 3. Expected game outcome closeness to 50%
    double ratingDiff = std::abs(player1.rating - player2.rating);
    double avgDeviation = (player1.deviation + player2.deviation) / 2.0;
    double expectedScore = calculateExpectedScore(player1, player2);

    // Normalize factors to 0-1 range
    double ratingFactor = std::exp(-ratingDiff / RATING_DIFFERENCE_SCALE);
    double deviationFactor = std::exp(-avgDeviation / AVERAGE_DEVIATION_SCALE);
    double balanceFactor = 1.0 - std::abs(expectedScore - 0.5);

    // Combine factors with weights
    return (0.4 * ratingFactor + 0.3 * deviationFactor + 0.3 * balanceFactor);
}

double Glicko2::calculateExpectedScore(const Rating& player1, const Rating& player2) {
    double mu1 = toGlicko2Scale(player1.rating);
    double mu2 = toGlicko2Scale(player2.rating);
    double phi2 = convertToGlicko2Deviation(player2.deviation);
    return E(mu1, mu2, phi2);
}

double Glicko2::computeVariance(const std::vector<MatchResult>& results,
                               const Rating& player) {
    double sum = 0.0;
    double mu = toGlicko2Scale(player.rating);
    for (const auto& result : results) {
        double oppMu = toGlicko2Scale(result.opponent.rating);
        double oppPhi = convertToGlicko2Deviation(result.opponent.deviation);
        double gPhi = g(oppPhi);
        double E = 1.0 / (1.0 + std::exp(-gPhi * (mu - oppMu)));
        sum += (gPhi * gPhi * E * (1 - E));
    }
    return sum > 0.0 ? 1.0 / sum : 999999.0;
}

double Glicko2::computeDelta(const std::vector<MatchResult>& results,
                            const Rating& player,
                            double variance) {
    double sum = 0.0;
    double mu = toGlicko2Scale(player.rating);
    for (const auto& result : results) {
        double oppMu = toGlicko2Scale(result.opponent.rating);
        double oppPhi = convertToGlicko2Deviation(result.opponent.deviation);
        sum += g(oppPhi) * (result.score - E(mu, oppMu, oppPhi));
    }
    return variance * sum;
}

double Glicko2::updateVolatility(const Rating& rating,
                                double delta,
                                double variance) {
    double phi = convertToGlicko2Deviation(rating.deviation);
    double sigma = rating.volatility;
    double phi2 = phi * phi;

    // Implementation of the iteration algorithm
    auto f = [&](double x) {
        double ex = std::exp(x);
        double num = ex * (delta * delta - phi2 - variance - ex);
        double den = 2.0 * std::pow(phi2 + ex, 2);
        return num / den - (x - std::log(sigma * sigma)) / (TAU * TAU);
    };

    double a = std::log(sigma * sigma);
    double A = a;
    double B = delta * delta > phi2 + variance ?
               std::log(delta * delta - phi2 - variance) :
               a - TAU;
    double fA = f(A);
    double fB = f(B);

    // Illinois algorithm
    for (int i = 0; i < MAX_ITERATIONS && std::abs(B - A) > EPSILON; ++i) {
        double C = A + (A - B) * fA / (fB - fA);
        double fC = f(C);
        if (fC * fB < 0) {
            A = B;
            fA = fB;
        } else {
            fA *= 0.5;
        }
        B = C;
        fB = fC;
    }
    return std::exp(A / 2.0);
}