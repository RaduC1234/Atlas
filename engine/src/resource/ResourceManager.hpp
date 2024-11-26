#pragma once

#include <nlohmann/json.hpp>

#include "ResourceCache.hpp"
#include "Animation.hpp"

class ResourceManager {
public:
    template<typename T>
    static std::shared_ptr<T> load(const std::string& key, const std::string& path,
                                   typename ResourceCache<T>::Loader loader = nullptr) {
        return getResourceCache<T>().load(key, path, loader);
    }

    template<typename T>
    static std::shared_ptr<T> get(const std::string& key) {
        return getResourceCache<T>().handle(key);
    }

    template<typename T>
    static void release(const std::string& key) {
        getResourceCache<T>().release(key);
    }

    template<typename T>
    static void clear() {
        getResourceCache<T>().clear();
    }

    static void clearAll() {
        cacheMap().clear();
    }

    // New loadAnimations method
    static std::unordered_map<std::string, std::shared_ptr<Animation>> loadAnimations(const std::string& filePath) {
        std::unordered_map<std::string, std::shared_ptr<Animation>> animations;

        try {
            // Open the JSON file
            std::ifstream file(filePath);
            if (!file.is_open()) {
                throw std::runtime_error("Failed to open animation file: " + filePath);
            }

            // Parse the JSON
            nlohmann::json jsonData;
            file >> jsonData;

            // Load animations
            for (const auto& [key, value] : jsonData.items()) {
                animations[key] = std::make_shared<Animation>(key, jsonData);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error loading animations: " << e.what() << "\n";
        }

        return animations;
    }

private:
    template<typename T>
    static ResourceCache<T>& getResourceCache() {
        auto& map = cacheMap();
        std::type_index typeId = std::type_index(typeid(T));

        if (map.find(typeId) == map.end()) {
            map[typeId] = std::make_shared<ResourceCache<T>>();
        }

        return *std::static_pointer_cast<ResourceCache<T>>(map[typeId]);
    }

    static std::unordered_map<std::type_index, std::shared_ptr<void>>& cacheMap() {
        static std::unordered_map<std::type_index, std::shared_ptr<void>> instance;
        return instance;
    }
};
