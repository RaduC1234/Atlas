#ifndef MATCHMAKING_DLL_HPP
#define MATCHMAKING_DLL_HPP

#ifdef MATCHMAKER_EXPORTS
#define MATCHMAKING_API __declspec(dllexport)
#else
#define MATCHMAKING_API __declspec(dllimport)
#endif

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

extern "C" {
    MATCHMAKING_API void getDefaultRating(Rating* rating);
    MATCHMAKING_API void evaluateMatch(const Rating* players, int numPlayers, MatchQuality* result);
    MATCHMAKING_API bool canPlayerJoin(const Rating* newPlayer,
                                       const Rating* existingPlayers,
                                       int numExistingPlayers,
                                       char* reason,
                                       int reasonSize);
    MATCHMAKING_API void getValidRatingRange(const Rating* existingPlayers,
                                             int numExistingPlayers,
                                             double* minRating,
                                             double* maxRating,
                                             bool* success,
                                             char* reason,
                                             int reasonSize);
    MATCHMAKING_API void updateRatings(Rating* currentRating,
                                       const Rating* opponents,
                                       const double* results,
                                       int numOpponents,
                                       bool* success,
                                       char* errorMsg,
                                       int errorMsgSize);
}

#endif // MATCHMAKING_DLL_HPP
