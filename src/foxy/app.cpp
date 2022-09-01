#include "app.hpp"

#include "foxy/core/window.hpp"

namespace foxy {
  App::App(const foxy::AppCreateInfo& properties) {
    FOXY_ASSERT(!instantiated_) << "Attempted second instantiation of foxy::App";
    instantiated_ = true;

    const std::string greeting{R"([]=============================[]
||  ______ ______   ___     __ ||
|| |  ____/ __ \ \ / \ \   / / ||
|| | |__ | |  | \ V / \ \_/ /  ||
|| |  __|| |  | |> <   \   /   ||
|| | |   | |__| / . \   | |    ||
|| |_|    \____/_/ \_\  |_|    ||
||                             ||
[]=============================[])"};

    std::clog << greeting << '\n';

    Log::init();
    FOXY_INFO << "Foxy startup: Kon kon kitsune! Hi, friends!";

    window_ = std::make_unique<Window>(WindowCreateInfo{
        properties.title,
        properties.width,
        properties.height,
        properties.vsync,
        properties.fullscreen,
        properties.borderless,
    });

    window_->set_hidden(false);
  }

  void App::game_loop() {
    while (window_->running()) {
      window_->poll_events();
    }
  }

  App::~App() {
    instantiated_ = false;
    FOXY_INFO << "Foxy shutdown: Otsukon deshita! Bye bye!";
  }
}