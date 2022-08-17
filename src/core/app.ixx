﻿module;

#include <foxy/log_macros.hpp>

export module foxy.app;

import foxy.log;

import <iostream>;
import <string>;

namespace foxy {
  export void greet() {
    std::cout << "hi, friends!\n";
  }

  export struct WindowProperties {
    const std::string title{ "FOXY FRAMEWORK" };
    const int width{ 800 };
    const int height{ 450 };
    //const Renderer::API renderApi{ Renderer::API::OpenGL };
    const bool vSync{ true };
    const bool fullScreen{ false };
    const bool borderless{ false };
  };

  export class App final {
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

  private:
    void gameLoop() {

    }
  };
}

// if you are reading this, know that Shirakami Fubuki is STILL the best fox waifu friend