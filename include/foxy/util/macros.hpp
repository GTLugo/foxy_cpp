//
// Created by galex on 8/21/2022.
//

#pragma once

#include "foxy/util/log_macros.hpp"

#ifndef FOXY_DEBUG_MODE
  #define FOXY_DEBUG_BREAK
#else
  #ifdef _WIN32 // Windows
    #define FOXY_DEBUG_BREAK __debugbreak()
  #else
    #include <csignal>
    #ifdef SIGTRAP // POSIX
      #define FOXY_DEBUG_BREAK raise(SIGTRAP)
    #else // Other
      #define FOXY_DEBUG_BREAK raise(SIGABRT)
    #endif
  #endif
#endif

#ifdef FOXY_ENABLE_ASSERTS
  #define FOXY_ASSERT(x) if(!(x)) FOXY_FATAL << "FAILED ASSERT: "
#else
  #include <iostream>
  #define FOXY_ASSERT(x) if(false) std::clog
#endif

#define BIT(x) (1 << x)
#define FOXY_BIT_COMPARE_TO(x, y) (x & y) == y
#define FOXY_STRINGIFY_VAL(x) FOXY_STRINGIFY(x)
#define FOXY_STRINGIFY(x) #x
#define FOXY_LAMBDA_INS(fn, instance) [objPtr = instance](auto&&... args) { return objPtr->fn(std::forward<decltype(args)>(args)...); }
#define FOXY_LAMBDA(fn) FOXY_LAMBDA_INS(fn, this)

#if defined(_WIN32) and not defined(FOXY_DEBUG_MODE)
#define WINMAIN_DEFERRED_TO_MAIN \
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {\
  return main(__argc, __argv);\
}
#else
#define WINMAIN_DEFERRED_TO_MAIN int fubuki_is_cute() { return 0; }
#endif

#if defined(_WIN32) and not defined(FOXY_DEBUG_MODE)
#define WINMAIN_DEFERRED_TO_FOXY_MAIN \
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {\
  return main(__argc, reinterpret_cast<std::int8_t**>(__argv));\
}
#else
#define WINMAIN_DEFERRED_TO_FOXY_MAIN int fubuki_is_cute() { return 0; }
#endif