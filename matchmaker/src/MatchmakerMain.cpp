#include "Matchmaking.hpp"
#include <Windows.h>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <string>
#include <iomanip>

namespace fs = std::filesystem;

// Type definitions for DLL function pointers
typedef void (*GetDefaultRatingFunc)(Rating*);
typedef void (*EvaluateMatchFunc)(const Rating*, int, MatchQuality*);
typedef bool (*CanPlayerJoinFunc)(const Rating*, const Rating*, int, char*, int);
typedef void (*GetValidRatingRangeFunc)(const Rating*, int, double*, double*, bool*, char*, int);
typedef void (*UpdateRatingsFunc)(Rating*, const Rating*, const double*, int, bool*, char*, int);

struct MatchmakingDLL {
    HMODULE handle;
    GetDefaultRatingFunc getDefaultRating;
    EvaluateMatchFunc evaluateMatch;
    CanPlayerJoinFunc canPlayerJoin;
    GetValidRatingRangeFunc getValidRatingRange;
    UpdateRatingsFunc updateRatings;
};

bool loadMatchmakingDLL(MatchmakingDLL& dll, std::string& status) {
    std::stringstream ss;

    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    fs::path exePath = buffer;
    fs::path exeDir = exePath.parent_path();

    std::vector<fs::path> searchPaths = {
        exeDir / "matchmaker.dll",
        exeDir / "server" / "matchmaker.dll",
        exeDir / ".." / "server" / "matchmaker.dll",
        exeDir / ".." / "bin" / "matchmaker.dll"
    };

    ss << "DLL Distribution Status:\n";
    ss << "Searching for matchmaker.dll...\n";

    fs::path dllPath;
    bool found = false;

    for (const auto& path : searchPaths) {
        if (fs::exists(path)) {
            dllPath = path;
            found = true;
            break;
        }
    }

    if (!found) {
        ss << "ERROR: matchmaker.dll not found!\n";
        status = ss.str();
        return false;
    }

    ss << "Found DLL at: " << dllPath.string() << "\n";

    dll.handle = LoadLibraryA(dllPath.string().c_str());
    if (!dll.handle) {
        ss << "ERROR: Failed to load DLL!\n";
        status = ss.str();
        return false;
    }

    dll.getDefaultRating = (GetDefaultRatingFunc)GetProcAddress(dll.handle, "getDefaultRating");
    dll.evaluateMatch = (EvaluateMatchFunc)GetProcAddress(dll.handle, "evaluateMatch");
    dll.canPlayerJoin = (CanPlayerJoinFunc)GetProcAddress(dll.handle, "canPlayerJoin");
    dll.getValidRatingRange = (GetValidRatingRangeFunc)GetProcAddress(dll.handle, "getValidRatingRange");
    dll.updateRatings = (UpdateRatingsFunc)GetProcAddress(dll.handle, "updateRatings");

    if (!dll.getDefaultRating || !dll.evaluateMatch || !dll.canPlayerJoin ||
        !dll.getValidRatingRange || !dll.updateRatings) {
        ss << "ERROR: Failed to load one or more required functions!\n";
        FreeLibrary(dll.handle);
        status = ss.str();
        return false;
    }

    ss << "SUCCESS: DLL loaded successfully.\n\n";
    status = ss.str();
    return true;
}

void testMatchmaking(const MatchmakingDLL& dll) {
    std::cout << "Testing 4-Player Matchmaking:\n";
    std::cout << "------------------------------\n\n";

    // Create 4 players with different ratings
    Rating players[4];
    dll.getDefaultRating(&players[0]); // Default 1500
    dll.getDefaultRating(&players[1]);
    dll.getDefaultRating(&players[2]);
    dll.getDefaultRating(&players[3]);

    // Set different ratings to simulate various skill levels
    players[1].rating = 1600.0;  // Slightly above average
    players[2].rating = 1400.0;  // Slightly below average
    players[3].rating = 1550.0;  // Above average

    // Print player ratings
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "Player Ratings:\n";
    for (int i = 0; i < 4; i++) {
        std::cout << "Player " << (i + 1) << ": " << players[i].rating
                  << " (RD: " << players[i].deviation << ")\n";
    }
    std::cout << "\n";

    // Test if players can join one by one
    char reason[256];
    std::cout << "Testing Sequential Join:\n";
    for (int i = 1; i < 4; i++) {
        bool canJoin = dll.canPlayerJoin(&players[i], players, i, reason, sizeof(reason));
        std::cout << "Can Player " << (i + 1) << " join? "
                  << (canJoin ? "Yes" : "No");
        if (!canJoin) {
            std::cout << " - Reason: " << reason;
        }
        std::cout << "\n";
    }
    std::cout << "\n";

    // Test match quality
    MatchQuality quality;
    dll.evaluateMatch(players, 4, &quality);

    std::cout << "Match Quality Assessment:\n";
    std::cout << "Valid Match: " << (quality.isValid ? "Yes" : "No") << "\n";
    std::cout << "Quality Score: " << quality.quality << "\n";
    std::cout << "Skill Spread: " << quality.skillSpread << "\n";
    if (!quality.isValid) {
        std::cout << "Invalid Match Reason: " << quality.reason << "\n";
    }
    std::cout << "\n";

    // Test rating updates after a match
    std::cout << "Testing Rating Updates:\n";
    // Simulate match results (win percentages against other players)
    double results[] = {1.0, 0.0, 1.0};  // Beat player 2 and 4, lost to player 3
    bool success;
    char errorMsg[256];

    Rating updatedRating = players[0];
    dll.updateRatings(&updatedRating, &players[1], results, 3, &success, errorMsg, sizeof(errorMsg));

    if (success) {
        std::cout << "Player 1 Rating Update:\n";
        std::cout << "Old Rating: " << players[0].rating << "\n";
        std::cout << "New Rating: " << updatedRating.rating << "\n";
        std::cout << "Change: " << (updatedRating.rating - players[0].rating) << "\n";
    } else {
        std::cout << "Failed to update ratings: " << errorMsg << "\n";
    }
}

int main() {
    MatchmakingDLL dll = {};
    std::string status;

    if (!loadMatchmakingDLL(dll, status)) {
        std::cerr << status;
        std::cerr << "\nPress Enter to exit...";
        std::cin.get();
        return 1;
    }

    std::cout << status;

    testMatchmaking(dll);

    if (dll.handle) {
        FreeLibrary(dll.handle);
    }

    std::cout << "\nPress Enter to exit...";
    std::cin.get();
    return 0;
}