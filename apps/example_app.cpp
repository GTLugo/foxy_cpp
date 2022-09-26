#include <foxy/framework.hpp>
REDIRECT_WINMAIN_TO_MAIN

struct ExampleApp: foxy::App {
  std::string waifu{ "Fubuki" };
  int hololive_members{ 71 };

  ExampleApp()
  : App{ CreateInfo{
      .title = "Foxy Example App"
    } }
  {
    add_function_to_stage(Stage::Start, FOXY_LAMBDA(start));
    add_function_to_stage(Stage::EarlyUpdate, FOXY_LAMBDA(update));
    add_function_to_stage(Stage::Stop, FOXY_LAMBDA(stop));
  }

  void start(App&)
  {
    koyote::Log::info("My favorite out of all {} hololive members is {}", hololive_members, waifu);
  }

  void update(App&)
  {
    const double delta_time{ koyote::Time::delta<koyote::secs>() };

    static double timer{ 0 };
    static koyote::u64 counter{ 0 };
    if (1. <= (timer += delta_time)) {
      koyote::Log::info("uwu {} - {}", counter++, timer);
      timer = 0;
    }
  }

  void stop(App&)
  {
    // koyote::Log::fatal("Crash test!");
  }
};

auto main(const int, char**) -> int
{
  try {
    koyote::Log::enable_backtrace(32);
    ExampleApp{}.run();
    return EXIT_SUCCESS;
  } catch (const std::exception& e) {
    koyote::Log::fatal(e.what());
    return EXIT_FAILURE;
  }
}