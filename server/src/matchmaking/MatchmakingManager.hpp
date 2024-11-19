#pragma once

#include <Windows.h>
#include <entt/entity/entity.hpp>

#include "core/Log.hpp"

constexpr auto dllName = "matchmaking.dll";

class MatchmakingManager {
public:
    static void init() {
#ifdef WIN32
        hDLL = LoadLibrary(dllName);
        if (!hDLL) {
            AT_FATAL("Failed to load {0}", dllName);
        }

        // add functions here
        getDefaultValuesFunc = GetProcAddress(hDLL, "getDefaultValues");

        if (!getDefaultValuesFunc) {
            shutdown();
            AT_ERROR("Failed to load functions from {0}", dllName);
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

    static void getDefaultValues(double &rating, double &rd, double &volatility) {
#ifdef WIN32
        typedef void (*FuncType)(double *, double *, double *);
        FuncType func = reinterpret_cast<FuncType>(getDefaultValuesFunc);
        func(&rating, &rd, &volatility);
#endif
    }

private:
#ifdef WIN32
    static HMODULE hDLL{nullptr};

    // add pointers
    static FARPROC getDefaultValuesFunc{nullptr};
#endif
};
