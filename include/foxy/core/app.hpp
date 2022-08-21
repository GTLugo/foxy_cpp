//
// Created by galex on 8/21/2022.
//

#pragma once

#include "foxy/util/log.hpp"
#include "foxy/util/util.hpp"

#include <iostream>
#include <string>

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
    explicit App(const WindowProperties& props = {}) {
      Log::init();
      FOXY_INFO << "Foxy startup: Kon kon kitsune! Hi, friends!";
    }

    ~App() {
      FOXY_INFO << "Foxy shutdown: Otsukon deshita! Bye bye!";
    }

    void run() {

    }

    void close() {

    }
  private:
    void game_loop() {

    }
  };
}
