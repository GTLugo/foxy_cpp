//
// Created by galex on 8/21/2022.
//

#pragma once

#include "window.hpp"
#include "foxy/core/event_system/event.hpp"

namespace foxy {
  class App final: NoCopyOrMove {
  public:
    explicit App(const Window::Properties& properties = {});
    ~App();

    void run();
    void close();

    App& add_global_data();
    App& add_step_before();
    App& add_system_to_step();

  private:
    static inline bool instantiated_{ false };
    bool running_{ true };

    unique<Window> window_{ nullptr };

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
