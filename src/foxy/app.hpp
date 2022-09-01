//
// Created by galex on 8/21/2022.
//

#pragma once

#include "foxy/internal/includes.hpp"

#include "foxy/core/event_system/event.hpp"

namespace foxy {
  class Window;

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
    App(const AppCreateInfo& properties = {});

    ~App();

    auto add_global_data() -> App& {
      return *this;
    }

    auto add_step_before() -> App& {
      return *this;
    }

    auto add_system_to_step() -> App& {
      return *this;
    }

    void run() {
      game_loop();
    }

  private:
    static inline bool instantiated_{ false };
    bool running_{ true };

    Unique<Window> window_;

    // Main Thread events
    Event<> main_awake_event_;
    Event<> main_start_event_;
    Event<> main_poll_event_;
    Event<> main_update_event_;
    Event<> main_stop_event_;
    // Game Thread events
    Event<> game_awake_event_;
    Event<> game_start_event_;
    Event<> game_tick_event_;
    Event<> game_update_event_;
    Event<> game_stop_event_;

    void game_loop();
  };
}
