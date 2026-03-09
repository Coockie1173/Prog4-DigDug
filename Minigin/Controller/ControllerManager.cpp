#include "ControllerManager.h"

#if defined(_WIN32) && !defined(__EMSCRIPTEN__)
#include "ControllerManager_XInput.cpp"
#else
#include "ControllerManager_SDL.cpp"
#endif