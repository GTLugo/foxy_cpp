#include <foxy/framework.hpp>
REDIRECT_WINMAIN_TO_MAIN

struct ExampleApp {
  std::string waifu{ "Fubuki" };
  int hololive_members{ 71 };

  void start(foxy::App& app) {
    FOXY_DEBUG << "My favorite out of all " << hololive_members << " hololive members is " << waifu;
  }

  void update(foxy::App& app) {
    
  }

  void run() {
    foxy::App{foxy::App::CreateInfo{
      .title = "Foxy Example App"
    }}.add_function_to_stage(foxy::App::Stage::Start, FOXY_LAMBDA(start))
      .add_function_to_stage(foxy::App::Stage::EarlyUpdate, FOXY_LAMBDA(update))
      .add_stage_before()
      .run();
  }
};

auto main(int, char**) -> int {
  try {
    ExampleApp{}.run();
    return EXIT_SUCCESS;
  } catch (const std::exception& e) {
    FOXY_FATAL << "APP EXCEPTION: " << e.what();
    return EXIT_FAILURE;
  }
}