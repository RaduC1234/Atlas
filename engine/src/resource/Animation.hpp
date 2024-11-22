//
// Created by Karina on 11/22/2024.
//

#pragma once

#include <core/Core.hpp>

class Animation
{
public:
    Animation(const std::string& animationName, const std::string& filePath) {
        try {
            auto customLoad = [&](const std::string& key, const std::string& path) -> nlohmann::json {
                auto jsonResource = ResourceManager::load<nlohmann::json>(key, path);
                if (!jsonResource) {
                    throw std::runtime_error("Failed to load JSON resource: " + key);
                }
                return *jsonResource;
            };

            nlohmann::json jsonData = customLoad(animationName, filePath);

            if (jsonData.contains(animationName)) {
                const auto& animData = jsonData.at(animationName);
                m_frames = animData.at("frames").get<std::vector<std::string>>();
                m_frameDuration = animData.at("frameDuration").get<float>();
                m_loop = animData.at("loop").get<bool>();
                m_name = animationName;
            } else {
                throw std::runtime_error("Animation name not found in JSON: " + animationName);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error loading animation: " << e.what() << "\n";
        }
    }

    const std::string getName() const { return m_name; };
    const std::vector<std::string>& getFrames() const { return m_frames; };
    float getFrameDuration() const { return m_frameDuration; };
    bool isLooping() const { return m_loop; };

private:
    std::string m_name;
    std::vector<std::string> m_frames;
    float m_frameDuration = 0.0f;
    bool m_loop = false;
};
