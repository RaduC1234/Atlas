#pragma once

#include "ResourceCache.hpp"

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
