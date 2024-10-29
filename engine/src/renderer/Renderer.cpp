#include "Renderer.hpp"

#ifdef ATLAS_BACKEND_SFML
#include "backend/SFMLRenderer.hpp"
#include "backend/SFMLWindow.hpp"
#endif


Scope<Renderer> Renderer::create(void *nativeWindow, glm::vec4 clearColor) {
#ifdef ATLAS_BACKEND_SFML
    return CreateScope<SFMLRenderer>(static_cast<SFMLWindow*>(nativeWindow), clearColor);
#else
    AT_ASSERT(false, "No Renderer Backend")
    return nullptr;
#endif
}
