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
  }.add_system_to_step()
      .add_global_data()
      .add_step_before()
      .run();
}

export WINMAIN_DEFERRED_TO_FOXY_MAIN
