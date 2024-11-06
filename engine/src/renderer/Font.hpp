#pragma once

#include <freetype/freetype.h>

#include "core/Core.hpp"

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

    Font(const std::string &filePath) : filePath(filePath) {

        if(!isLoaded) {
            loadLibs();
            isLoaded = true;
        }

        load();
    }

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

    void load() {

        if (!isLoaded) {
            loadLibs();
            isLoaded = true;
        }

        FT_Face face;
        if (FT_New_Face(ft, filePath.c_str(), 0, &face)) {
            AT_WARN("ERROR::FREETYPE: Failed to loadLibs font");
        }

        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for (unsigned char c = 0; c < 128; c++) {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                AT_WARN("ERROR::FREETYPE: Failed to loadLibs Glyph for character: {0}", c);
                continue;
            }

            uint32_t texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    face->glyph->bitmap.width,
                    face->glyph->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            Character character = {
                    texture,
                    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                    static_cast<unsigned int>(face->glyph->advance.x)
            };

            this->characters.insert(std::pair<char, Character>(c, character));

            glBindTexture(GL_TEXTURE_2D, 0);
        }

        FT_Done_Face(face);
    }

    static void loadLibs() {

        if (FT_Init_FreeType(&ft)) {
            AT_WARN("ERROR::FREETYPE: Could not init FreeType Library");
            return;
        }
    }
    std::string filePath;
    std::map<char, Character> characters;

    inline static FT_Library ft;
    inline static bool isLoaded = false;
};
