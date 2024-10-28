#include "Window.hpp"

#ifdef ATLAS_BACKEND_SDL
#include "backend/SDLWindow.hpp"
#endif

#ifdef ATLAS_BACKEND_SFML
#include "backend/SFMLWindow.hpp"
#endif

Scope<Window> Window::create(const WindowProps &props) {
#ifdef ATLAS_BACKEND_SDL
    return CreateScope<SDLWindow>(props);
#elif ATLAS_BACKEND_SFML
    return CreateScope<SFMLWindow>(props);
#else
    AT_ASSERT(false, "No window backend")
    return nullptr;
#endif
}


