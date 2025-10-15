#include "os.h"

namespace QtCommon::OS {
void PreventOSSleep() {
#ifdef _WIN32
    SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
#elif defined(HAVE_SDL2)
    SDL_DisableScreenSaver();
#endif
}

void AllowOSSleep() {
#ifdef _WIN32
    SetThreadExecutionState(ES_CONTINUOUS);
#elif defined(HAVE_SDL2)
    SDL_EnableScreenSaver();
#endif
}

}
