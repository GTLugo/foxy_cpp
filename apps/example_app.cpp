#include <foxy/framework.hpp>
REDIRECT_WINMAIN_TO_MAIN

auto main(int, char**) -> int {
  try {
    foxy::App{{
      .title = "Foxy Example App"
    }}.add_system_to_step()
        .add_global_data()
        .add_step_before()
        .run();
    return EXIT_SUCCESS;
  } catch (const std::exception& e) {
    std::clog << e.what();
    return EXIT_FAILURE;
  }
}