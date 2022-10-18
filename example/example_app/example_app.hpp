#pragma once
#include <foxy/framework.hpp>

struct ExampleSystem : public fx::System<fx::Time, fx::i32> {
  double timer{ 0 };
  fx::u64 counter{ 1 };

  void execute() override
  {
    // if (1. <= (timer += time.delta<fx::secs>())) {
    //   std::string extra_message{};
    //   if (counter == 10) {
    //     extra_message = ", kawaii~oooh! ...chan!";
    //   } else if (counter % 3 == 0) {
    //     extra_message = ", Fubuki!";
    //   }
    //
    //   fx::Log::info("{} fox{}", counter, extra_message);
    //
    //   counter = (counter % 10) + 1;
    //   timer = 0;
    // }
  }
};

struct ExampleApp : fx::App {
  using Time = fx::Time;

  std::string waifu{ "Fubuki" };
  int hololive_members{ 71 };

  ExampleApp():
    App{ CreateInfo{.title = "Foxy Example App" } }
  {
    add_function_to_stage(Stage::Start, FOXY_LAMBDA(start));
    add_function_to_stage(Stage::Update, FOXY_LAMBDA(update));
  }

  void start(App&, const Time&)
  {
    fx::Log::info("My favorite out of all {} hololive members is {}", hololive_members, waifu);

    fx::Log::info("Component::ID for Name: {}", to_string(fx::Component::id<fx::components::Name>()));
    fx::Log::info("Component::ID for Transform: {}", to_string(fx::Component::id<fx::components::Transform>()));
    fx::Log::info("Component::ID for Mesh: {}", to_string(fx::Component::id<fx::components::Mesh>()));
    const auto example_entity{
      fx::Entity{"example-entity"}
        .add<fx::components::Transform>(fx::components::Transform{
            .position = { 1.f, 1.f, 1.f }
          })
        .add<fx::components::Mesh>()
    };

    if (example_entity.has<fx::components::Name, fx::components::Transform>()) {
      fx::Log::info("Found all components! Name: {}", example_entity.get<fx::components::Name>().value);
    } else {
      fx::Log::info("Missing one or more components!");
    }
  }

  void update(App&, const Time& time)
  {

  }
};
