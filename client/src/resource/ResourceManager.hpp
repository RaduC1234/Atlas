#pragma once

#include "ResourceCache.hpp"

class ResourceManager {
public:
    template<typename T>
    static std::shared_ptr<T> load(const std::string &key, const std::string &path,
                                   typename ResourceCache<T>::Loader loader = nullptr) {
        return getResourceCache<T>().load(key, path, loader);
    }

    template<typename T>
    static std::shared_ptr<T> get(const std::string &key) {
        return getResourceCache<T>().handle(key);
    }

    template<typename T>
    static void release(const std::string &key) {
        getResourceCache<T>().release(key);
    }

    template<typename T>
    static void clear() {
        getResourceCache<T>().clear();
    }

    static void clearAll() {
        cacheMap().clear();
    }

    template<typename T, typename... Extensions>
    static void loadFromDirectory(const std::string &directoryPath, typename ResourceCache<T>::Loader loader, Extensions... extensions) {
        namespace fs = std::filesystem;

        std::vector<std::string> extensionList = {extensions...};

        try {
            for (const auto &entry: fs::recursive_directory_iterator(directoryPath)) {
                // Skip directories named "old" (case-insensitive)
                if (entry.is_directory() &&
                    fs::path(entry).filename().string() == "old" &&
                    fs::path(entry).filename().string() == "cursors") {
                    continue; // Skip this directory
                }

                if (entry.is_regular_file()) {
                    std::string filePath = entry.path().string();
                    std::string extension = entry.path().extension().string();
                    std::string fileName = entry.path().stem().string(); // Filename without extension

                    // Check if the file has a valid extension
                    if (std::find(extensionList.begin(), extensionList.end(), extension) != extensionList.end()) {
                        // Use the ResourceManager's load method to load the resource
                        load<T>(fileName, filePath, loader);
                    }
                }
            }
        } catch (const std::exception &e) {
            AT_ERROR("Error while loading resources from directory: {0}", e.what());
        }
    }

private:
    template<typename T>
    static ResourceCache<T> &getResourceCache() {
        auto &map = cacheMap();
        std::type_index typeId = std::type_index(typeid(T));

        if (map.find(typeId) == map.end()) {
            map[typeId] = std::make_shared<ResourceCache<T> >();
        }

        return *std::static_pointer_cast<ResourceCache<T> >(map[typeId]);
    }

    static std::unordered_map<std::type_index, std::shared_ptr<void> > &cacheMap() {
        static std::unordered_map<std::type_index, std::shared_ptr<void> > instance;
        return instance;
    }
};
