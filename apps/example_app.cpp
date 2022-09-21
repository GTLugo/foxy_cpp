#include <foxy/framework.hpp>
REDIRECT_WINMAIN_TO_MAIN

struct ExampleApp {
  std::string waifu{ "Fubuki" };
  int hololive_members{ 71 };

  void start(foxy::App& app) {
    koyote::Log::info("My favorite out of all {} hololive members is {}", hololive_members, waifu);
  }

  void update(foxy::App& app) {
    double delta_time{ koyote::Time::delta<koyote::secs>() };

    static double timer{ 0 };
    static koyote::u64 counter{ 0 };
    if (1. <= (timer += delta_time)) {
      koyote::Log::info("uwu {} - {}", counter++, timer);
      timer = 0;
    }
  }

  void stop(foxy::App& app) {
    // koyote::Log::fatal("Crash test!");
  }

  void run(int argc, char** argv) {
    foxy::App{foxy::App::CreateInfo{
      .argc = argc,
      .argv = argv,
      .title = "Foxy Example App"
    }}.add_function_to_stage(foxy::App::Stage::Start, FOXY_LAMBDA(start))
      .add_function_to_stage(foxy::App::Stage::EarlyUpdate, FOXY_LAMBDA(update))
      .add_function_to_stage(foxy::App::Stage::Stop, FOXY_LAMBDA(stop))
      .run();
  }
};

auto main(int argc, char** argv) -> int {
  try {
    koyote::Log::enable_backtrace(32);
    ExampleApp{}.run(argc, argv);
    return EXIT_SUCCESS;
  } catch (const std::exception& e) {
    koyote::Log::fatal("{}", e.what());
    return EXIT_FAILURE;
  }
}