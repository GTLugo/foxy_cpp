module;
#define __SPECSTRINGS_STRICT_LEVEL 0 // workaround for annoying macro redef warnings

#include <easylogging++.h>

export module foxy;

export INITIALIZE_EASYLOGGINGPP

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
      el::Configurations config{ "res/foxy_cpp/log_config.conf" };
      el::Loggers::reconfigureAllLoggers(config);
      el::Helpers::setThreadName("main-thread");
      LOG(TRACE) << "hi, friends!";
    }

    ~App() {
      LOG(TRACE) << "bye, friends!";
    }

    void run() {

    }

  private:
    void gameLoop() {

    }

  };
}

// if you are reading this, know that Shirakami Fubuki is STILL the best fox waifu friend