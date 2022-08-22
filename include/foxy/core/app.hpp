//
// Created by galex on 8/21/2022.
//

#pragma once

namespace foxy {
  void greet() {
    std::cout << "hi, friends!\n";
  }

  struct WindowProperties {
    const std::string title{ "FOXY FRAMEWORK" };
    const int width{ 800 };
    const int height{ 450 };
    //const Renderer::API renderApi{ Renderer::API::OpenGL };
    const bool v_sync{ true };
    const bool full_screen{ false };
    const bool borderless{ false };
  };

  class App final {
  public:
    explicit App(const WindowProperties& props = {});
    ~App();

    void run();
    void close();

    App& add_global_data();
    App& add_step_before();
    App& add_system_to_step();

  private:
    void game_loop();
  };
}
