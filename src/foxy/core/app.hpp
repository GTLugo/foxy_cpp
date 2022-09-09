//
// Created by galex on 8/21/2022.
//

#pragma once

namespace foxy {
  struct AppCreateInfo {
    std::string title{ "FOXY FRAMEWORK" };
    i32 width{ 800 };
    i32 height{ 450 };
    bool vsync{ true };
    bool fullscreen{ false };
    bool borderless{ false };
  };

  class App {
  public:
    explicit App(const AppCreateInfo&& properties = {});
    ~App();

    void run();

    auto add_global_data() -> App&;
    auto add_step_before() -> App&;
    auto add_system_to_step() -> App&;

  private:
    class Impl;
    Unique<Impl> pImpl_;
  };
}
