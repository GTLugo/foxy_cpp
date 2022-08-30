module;

#include <foxy/foxy.hpp>

#ifdef __INTELLISENSE__
#include <app.ixx>
#endif

export module test_app;

#ifndef __INTELLISENSE__
import foxy_app;
#endif

using i32 = foxy::i32;
using i8 = foxy::i8;

export auto main(i32 argc, i8** argv) -> i32 {
  foxy::App{
    foxy::AppCreateInfo{
      .title = "Foxy App"
    }
  }.run();
}

export WINMAIN_DEFERRED_TO_FOXY_MAIN
