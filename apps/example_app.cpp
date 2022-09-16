#include <foxy/framework.hpp>
REDIRECT_WINMAIN_TO_MAIN

struct ExampleApp {
  std::string waifu{ "Fubuki" };
  int hololive_members{ 71 };

  void start(fox::App& app) {
    LOG(DEBUG) << "My favorite out of all " << hololive_members << " hololive members is " << waifu;
  }

  void update(fox::App& app) {
    double delta_time{ kyt::time::delta<kyt::secs>() };
  }

  void run() {
    fox::App{fox::App::CreateInfo{
      .title = "Foxy Example App"
    }}.add_function_to_stage(fox::App::Stage::Start, FOXY_LAMBDA(start))
      .add_function_to_stage(fox::App::Stage::EarlyUpdate, FOXY_LAMBDA(update))
      .run();
  }
};

auto main(int, char**) -> int {
  try {
    ExampleApp{}.run();
    return EXIT_SUCCESS;
  } catch (const std::exception& e) {
    LOG(FATAL) << "APP EXCEPTION: " << e.what();
    return EXIT_FAILURE;
  }
}