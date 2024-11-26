#pragma once

#include <core/Core.hpp>
#include "resource/ResourceManager.hpp"

class Animation {
public:
    Animation(const std::string& animationName, const std::string& filePath) {
        try {
            // Load the JSON file
            nlohmann::json jsonData;
            std::ifstream file(filePath);
            if (!file.is_open()) {
                throw std::runtime_error("Failed to open JSON file: " + filePath);
            }
            file >> jsonData;

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

    explicit Animation(const std::string& animationName)
        : m_name(animationName) {}

    const std::string& getName() const { return m_name; }
    const std::vector<std::string>& getFrames() const { return m_frames; }
    float getFrameDuration() const { return m_frameDuration; }
    bool isLooping() const { return m_loop; }

private:
    std::string m_name;
    std::vector<std::string> m_frames;
    float m_frameDuration = 0.0f;
    bool m_loop = false;
};
