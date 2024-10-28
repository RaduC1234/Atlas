#include "PlatformUtils.hpp"

#ifdef ATLAS_BACKEND_SDL
#include "SDL.h"
#endif

double Time::getTime() {
#ifdef ATLAS_BACKEND_SDL
    return SDL_GetTicks();
#endif
    return 0;
}

