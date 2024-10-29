#include "Texture.hpp"

#ifdef ATLAS_BACKEND_SFML
#include "backend/SFMLTexture.hpp"
#endif

Scope<Texture> Texture::create(const std::string &filepath) {
#ifdef ATLAS_BACKEND_SFML
    return CreateScope<SFMLTexture>(filepath);
#else
    AT_ASSERT(false, "No texture backend")
#endif
}
