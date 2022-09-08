#include "app.hpp"

#include "foxy/core/window.hpp"
#include "foxy/core/event_system/event.hpp"

namespace foxy {
  App::App(const foxy::AppCreateInfo& properties) {
    FOXY_ASSERT(!instantiated_) << "Attempted second instantiation of foxy::App";
    instantiated_ = true;
    dummy_log_ = std::make_unique<Log>();
    Log::init();

    FOXY_INFO << R"([]=============================[])";
    FOXY_INFO << R"(||  ______ ______   ___     __ ||)";
    FOXY_INFO << R"(|| |  ____/ __ \ \ / \ \   / / ||)";
    FOXY_INFO << R"(|| | |__ | |  | \ V / \ \_/ /  ||)";
    FOXY_INFO << R"(|| |  __|| |  | |> <   \   /   ||)";
    FOXY_INFO << R"(|| | |   | |__| / . \   | |    ||)";
    FOXY_INFO << R"(|| |_|    \____/_/ \_\  |_|    ||)";
    FOXY_INFO << R"(||                             ||)";
    FOXY_INFO << R"([]=============================[])";
    FOXY_INFO << "Foxy startup: Kon kon kitsune! Hi, friends!";
    #ifdef FOXY_DEBUG_MODE
    FOXY_INFO << "Build mode: DEBUG";
    #else
    FOXY_INFO << "Build mode: RELEASE";
    #endif

    window_ = std::make_unique<Window>(WindowCreateInfo{
        properties.title,
        properties.width,
        properties.height,
        properties.vsync,
        properties.fullscreen,
        properties.borderless,
    });

    window_->set_hidden(false);

    main_awake_event_ = std::make_unique<Event<>>();
    main_start_event_ = std::make_unique<Event<>>();
    main_poll_event_ = std::make_unique<Event<>>();
    main_update_event_ = std::make_unique<Event<>>();
    main_stop_event_ = std::make_unique<Event<>>();
    game_awake_event_ = std::make_unique<Event<>>();
    game_start_event_ = std::make_unique<Event<>>();
    game_tick_event_ = std::make_unique<Event<>>();
    game_update_event_ = std::make_unique<Event<>>();
    game_stop_event_ = std::make_unique<Event<>>();
  }

  void App::game_loop() {
    while (window_->running()) {
      window_->poll_events();
    }
  }

  App::~App() {
    instantiated_ = false;
  }
}