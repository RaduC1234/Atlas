#include "Font.hpp"

#ifdef ATLAS_BACKEND_SFML
#include "backend/SFMLFont.hpp"
#endif

Scope<Font> Font::create(const std::string &filePath) {
#ifdef ATLAS_BACKEND_SFML
    return CreateScope<SFMLFont>(filePath);
#else
    AT_ASSERT(false, "No font backend!");
    return nullptr;
#endif
}
