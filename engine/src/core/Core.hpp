#pragma once

#include <any>
#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>


#include "Keyboard.hpp"
#include "Mouse.hpp"

#include "Log.hpp"

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

#ifdef _WIN32
#ifdef ATLAS_BUILD_SHARED
#define ATLAS_API __declspec(dllexport)
#else
#define ATLAS_API __declspec(dllimport)
#endif
#else
#error "Atlas currently supports only Windows platforms."
#endif

#ifdef ATLAS_ENABLE_ASSERT
#define AT_ASSERT(X, ...) { if(!(X)) { AT_ERROR(__VA_ARGS__); __debugbreak(); } }
#else
#define AT_ASSERT(X, ...)
#endif

#define TRY_CATCH(TRY_CODE, CATCH_CODE) \
    try {                               \
        TRY_CODE;                       \
    } catch (const std::exception& e) {  \
        CATCH_CODE;                      \
    }

#define BIT(x) (1 << x)


template<typename T>
using Scope = std::unique_ptr<T>;

template<typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args &&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args &&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
using WeakRef = std::weak_ptr<T>;



inline std::vector<std::string> split(const std::string& str, char delimiter = ' ') {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(str);

    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

