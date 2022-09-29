#include <foxy/framework.hpp>
REDIRECT_WINMAIN_TO_MAIN

struct ExampleApp : fx::App {
  std::string waifu{ "Fubuki" };
  int hololive_members{ 71 };
  bool crash_test{ false };

  ExampleApp():
    App{
      CreateInfo{
        .title = "Foxy Example App"
      }
    }
  {
    add_function_to_stage(Stage::Start, FOXY_LAMBDA(start));
    add_function_to_stage(Stage::Update, FOXY_LAMBDA(update));
    add_function_to_stage(Stage::Stop, FOXY_LAMBDA(stop));
    add_function_to_stage(Stage::Asleep, FOXY_LAMBDA(asleep));
  }

  void start(App&)
  {
    fx::Log::info("My favorite out of all {} hololive members is {}", hololive_members, waifu);
  }

  void update(App&)
  {
    const double delta_time{ fx::Time::delta<fx::secs>() };

    static double timer{ 0 };
    static fx::u64 counter{ 1 };
    if (1. <= (timer += delta_time)) {
      if (counter > 10) {
        counter = 1;
      }

      std::string extra_message{};
      if (counter == 10) {
        extra_message = ", kawaii~oooh! ...chan!";
      } else if (counter % 3 == 0) {
        extra_message = ", Fubuki!";
      }

      fx::Log::info("{} fox{}", counter++, extra_message);
      timer = 0;
    }
  }

  void stop(App&)
  {
    if (crash_test) {
      fx::Log::info("Crash test incoming...");
    }
  }

  void asleep(App&)
  {
    if (crash_test) {
      fx::Log::fatal("Crash test!");
    }
  }
};

auto main(const int, char**) -> int
{
  try {
    fx::Log::debug_logging_setup();
    ExampleApp{}.run();
    return EXIT_SUCCESS;
  } catch (const std::exception& e) {
    fx::Log::fatal(e.what());
    return EXIT_FAILURE;
  }
}