#pragma once

#include <Atlas.hpp>

#include <freetype/freetype.h>
#include <glm/glm.hpp>

// https://learnopengl.com/In-Practice/Text-Rendering

struct Character {

    uint32_t textureID; // ID handle of the glyph texture
    glm::ivec2 size; // Size of glyph
    glm::ivec2 bearing; // Offset from baseline to left/top of glyph
    uint32_t advance; // Horizontal offset to advance to next glyph
};

class Font {
public:
    Font() = default;

    Font(const std::string &filePath);

/*    ~Font() {

        for (auto &character: characters)
            if (character.second.textureID)
                glDeleteTextures(1, &character.second.textureID);
    }*/

    const Character &getCharacter(char c) const {
        return characters.at(c);
    }

    bool operator==(const Font& other) const {
        return filePath == other.filePath;
    }

private:

    void load();

    static void loadLibs();

    std::string filePath;
    std::map<char, Character> characters;

    static FT_Library ft;
    static bool isLoaded;
};
