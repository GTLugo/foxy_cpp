//
// Created by galex on 8/21/2022.
//

module;

#include "foxy/internal/foxy_includes.hpp"

#ifdef __INTELLISENSE__
#include "util/util.ixx"
#include "util/log.ixx"
#include "core/window.ixx"
#include "core/event_system/event.ixx"
#endif

export module foxy_app;

#ifndef __INTELLISENSE__
export import foxy_util;
import foxy_log;
import foxy_window;
import foxy_events;
#endif

namespace foxy {
  export struct AppCreateInfo {
    std::string title{ "FOXY FRAMEWORK" };
    i32 width{ 800 };
    i32 height{ 450 };
    bool vsync{ true };
    bool fullscreen{ false };
    bool borderless{ false };
  };

  export class App {
  public:
    App(const AppCreateInfo& properties = {}) {
      FOXY_ASSERT(!instantiated_) << "Attempted second instantiation of foxy::App";
      instantiated_ = true;

      const std::string greeting{ R"([]=============================[]
||  ______ ______   ___     __ ||
|| |  ____/ __ \ \ / \ \   / / ||
|| | |__ | |  | \ V / \ \_/ /  ||
|| |  __|| |  | |> <   \   /   ||
|| | |   | |__| / . \   | |    ||
|| |_|    \____/_/ \_\  |_|    ||
||                             ||
[]=============================[])" };

      std::clog << greeting << '\n';

      Log::init();
      FOXY_INFO << "Foxy startup: Kon kon kitsune! Hi, friends!";

      window_ = make_unique<Window>(WindowCreateInfo{
        properties.title,
        properties.width,
        properties.height,
        properties.vsync,
        properties.fullscreen,
        properties.borderless,
        });

      window_->set_hidden(false);
    }

    ~App() {
      instantiated_ = false;
      FOXY_INFO << "Foxy shutdown: Otsukon deshita! Bye bye!";
    }

    App& add_global_data() {
      return *this;
    }

    App& add_step_before() {
      return *this;
    }

    App& add_system_to_step() {
      return *this;
    }

    void run() {
      game_loop();
    }

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

    void game_loop() {
      while (window_->running()) {
        window_->poll_events();
      }
    }
  };
}
