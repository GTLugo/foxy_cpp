//
// Created by galex on 8/21/2022.
//

#pragma once

namespace foxy {
  class Window;
  template<class... Args>
  class Event;
  class Log;

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
    Unique<Log> dummy_log_; // this just allows for logging upon full destruction of App

    Unique<Window> window_;

    // Main Thread events
    Unique<Event<>> main_awake_event_;
    Unique<Event<>> main_start_event_;
    Unique<Event<>> main_poll_event_;
    Unique<Event<>> main_update_event_;
    Unique<Event<>> main_stop_event_;
    // Game Thread events
    Unique<Event<>> game_awake_event_;
    Unique<Event<>> game_start_event_;
    Unique<Event<>> game_tick_event_;
    Unique<Event<>> game_update_event_;
    Unique<Event<>> game_stop_event_;

    void game_loop();
  };
}
