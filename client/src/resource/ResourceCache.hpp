#pragma once

#include <Atlas.hpp>

template<typename T>
class ResourceCache { // Do not split in cpp and hpp. It doesn't work, link error. The compiler needs the definitions.
public:
    using ResourceHandle = std::shared_ptr<T>;
    using Loader = std::function<ResourceHandle(const std::string &)>;

    ResourceCache(Loader defaultLoader = [](const std::string &path) {
        return std::make_shared<T>(path);
    }) : defaultLoader(std::move(defaultLoader)) {
    }

    ResourceHandle load(const std::string &key, const std::string &path, Loader loader = nullptr) {
        auto it = resources.find(key);
        if (it != resources.end())
            return it->second;

        ResourceHandle resource = (loader ? loader : defaultLoader)(path);
        resources[key] = resource;
        return resource;
    }

    ResourceHandle handle(const std::string &key) const {
        auto it = resources.find(key);
        return (it != resources.end()) ? it->second : nullptr;
    }

    void release(const std::string &key) {
        resources.erase(key);
    }

    void clear() {
        resources.clear();
    }

private:
    std::unordered_map<std::string, ResourceHandle> resources;
    Loader defaultLoader;
};
