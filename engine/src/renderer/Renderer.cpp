#include "Renderer.hpp"

#ifdef ATLAS_BACKEND_SDL
#include "backend/SDLRenderer.hpp"
#endif

Scope<Renderer> Renderer::create(void* nativeWindow) {
#ifdef ATLAS_BACKEND_SDL
    return CreateScope<SDLRenderer>(static_cast<SDL_Window*>(nativeWindow));
#else
    AT_ASSERT(false, "No Renderer Backend")
    return nullptr;
#endif
}
