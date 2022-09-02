#include <foxy/framework.hpp>

auto main(int, char**) -> int {
  foxy::App{{
    .title = "Foxy Example App"
  }}.add_system_to_step()
    .add_global_data()
    .add_step_before()
    .run();
}

WINMAIN_DEFERRED_TO_MAIN
