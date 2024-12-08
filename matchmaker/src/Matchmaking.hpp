#pragma once

#include <Atlas.hpp>

constexpr double TAU = 0.5;
constexpr double EPSILON = 0.000001;
constexpr int MAX_ITERATIONS = 100;

// Rating boundaries
constexpr double MIN_RATING = 100.0;
constexpr double MAX_RATING = 3000.0;
constexpr double MIN_RD = 30.0;
constexpr double MAX_RD = 350.0;

struct MatchQuality {
    double quality;
    double expectedOutcome;
};

EXTERN_C {

    ATLAS_API void getDefaultValues(double* rating, double* rd, double* vol) {
        *rating = 1500.0;
        *rd = 350.0;
        *vol = 0.06;
    }

    ATLAS_API double convertToGlicko2Scale(double rating) {
        return (rating - 1500.0) / 173.7178;
    }

    ATLAS_API double convertFromGlicko2Scale(double rating) {
        return (rating * 173.7178) + 1500.0;
    }

    // Match quality assessment
    ATLAS_API MatchQuality calculateMatchQuality(
        double rating1, double rd1,
        double rating2, double rd2)
    {
        MatchQuality result;


        double mu1 = convertToGlicko2Scale(rating1);
        double phi1 = rd1 / 173.7178;
        double mu2 = convertToGlicko2Scale(rating2);
        double phi2 = rd2 / 173.7178;


        double g_phi = 1.0 / sqrt(1.0 + 3.0 * phi2 * phi2 / (M_PI * M_PI));


        result.expectedOutcome = 1.0 / (1.0 + exp(-g_phi * (mu1 - mu2)));


        double ratingDiff = std::abs(rating1 - rating2);
        double combinedUncertainty = sqrt(rd1 * rd1 + rd2 * rd2);


        result.quality = 1.0 / (1.0 + ratingDiff / 400.0 + combinedUncertainty / 200.0);

        return result;
    }

    // Rating validation and adjustment
    ATLAS_API void validateAndAdjustRatings(double* rating, double* rd) {
        // Ensure rating stays within bounds
        *rating = std::max(MIN_RATING, std::min(MAX_RATING, *rating));

        // Ensure RD stays within bounds
        *rd = std::max(MIN_RD, std::min(MAX_RD, *rd));
    }

    // Calculate rating change preview
    ATLAS_API void calculateRatingChangePreview(
        double rating, double rd, double vol,
        double opponentRating, double opponentRd,
        double score, // 1.0 for win, 0.5 for draw, 0.0 for loss
        double* expectedRatingChange,
        double* expectedRdChange)
    {
        double tempRating = rating;
        double tempRd = rd;
        double tempVol = vol;


        updateRatings(&tempRating, &tempRd, &tempVol,
                     &opponentRating, &opponentRd, &score, 1);

        *expectedRatingChange = tempRating - rating;
        *expectedRdChange = tempRd - rd;
    }

    // Core rating update function
    ATLAS_API void updateRatings(
        double* rating,
        double* rd,
        double* volatility,
        const double* opponentRatings,
        const double* opponentRDs,
        const double* scores,
        int numOpponents
    ) {

        double mu = convertToGlicko2Scale(*rating);
        double phi = *rd / 173.7178;
        double sigma = *volatility;


        std::vector<double> g_phi(numOpponents);
        std::vector<double> E(numOpponents);
        double v = 0.0;

        for (int i = 0; i < numOpponents; i++) {
            double mu_j = convertToGlicko2Scale(opponentRatings[i]);
            double phi_j = opponentRDs[i] / 173.7178;

            g_phi[i] = 1.0 / sqrt(1.0 + 3.0 * phi_j * phi_j / (M_PI * M_PI));
            E[i] = 1.0 / (1.0 + exp(-g_phi[i] * (mu - mu_j)));

            v += (g_phi[i] * g_phi[i] * E[i] * (1.0 - E[i]));
        }
        v = 1.0 / v;

        // Calculate delta
        double delta = 0.0;
        for (int i = 0; i < numOpponents; i++) {
            delta += g_phi[i] * (scores[i] - E[i]);
        }
        delta *= v;


        double a = log(sigma * sigma);
        double A = a;
        double B = 0.0;

        if (delta * delta > phi * phi + v) {
            B = log(delta * delta - phi * phi - v);
        } else {
            B = a - TAU;
        }

        double f_A = computeF(A, delta, phi, v, a, TAU);
        double f_B = computeF(B, delta, phi, v, a, TAU);

        for (int i = 0; i < MAX_ITERATIONS && abs(B - A) > EPSILON; i++) {
            double C = A + (A - B) * f_A / (f_B - f_A);
            double f_C = computeF(C, delta, phi, v, a, TAU);

            if (f_C * f_A < 0) {
                B = A;
                f_B = f_A;
            } else {
                f_B *= 0.5;
            }

            A = C;
            f_A = f_C;
        }

        double sigma_new = exp(A / 2.0);

        // Update rating deviation
        double phi_star = sqrt(phi * phi + sigma_new * sigma_new);
        double phi_new = 1.0 / sqrt(1.0 / (phi_star * phi_star) + 1.0 / v);

        // Update rating
        double mu_new = mu + phi_new * phi_new * delta;


        *rating = convertFromGlicko2Scale(mu_new);
        *rd = phi_new * 173.7178;
        *volatility = sigma_new;

        // Ensure values stay within bounds
        validateAndAdjustRatings(rating, rd);
    }


    ATLAS_API double calculateRatingConfidence(double rd) {
        return 1.0 - (rd - MIN_RD) / (MAX_RD - MIN_RD);
    }
}


static double computeF(double x, double delta, double phi, double v, double a, double tau) {
    double ex = exp(x);
    return (ex * (delta * delta - phi * phi - v - ex) / (2.0 * pow(phi * phi + ex, 2))) -
           (x - a) / (tau * tau);
}

static double calculateWinProbability(double playerRating, double opponentRating) {
    return 1.0 / (1.0 + pow(10.0, (opponentRating - playerRating) / 400.0));
}