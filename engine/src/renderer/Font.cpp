#include "Font.hpp"

Font::Font(const std::string &filePath) : filePath(filePath) {

    if(!isLoaded) {
        loadLibs();
        isLoaded = true;
    }

    load();
}
