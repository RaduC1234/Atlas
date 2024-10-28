#include "Texture.hpp"

#ifdef ATLAS_BACKEND_SDL
#include "backend/SDLTexture.hpp"
#endif

Scope<Texture> Texture::create(const std::string &filesystem, void* renderer) {
#ifdef ATLAS_BACKEND_SDL
    return CreateScope<SDLTexture>(filesystem, static_cast<SDL_Renderer*>(renderer));
#endif
    AT_ASSERT(false, "No texture backend")
    return nullptr;
}
