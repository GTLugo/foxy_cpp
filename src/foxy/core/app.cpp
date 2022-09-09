#include "app.hpp"

#include "foxy/core/window.hpp"
#include "foxy/core/event_system/event.hpp"
#include "foxy/koyote/ecs.hpp"

namespace foxy {
  class App::Impl {
  public:
    explicit Impl(const foxy::AppCreateInfo& create_info)
      : dummy_log_{} {
      FOXY_ASSERT(!instantiated_) << "Attempted second instantiation of foxy::App";
      instantiated_ = true;

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
        create_info.title,
        create_info.width,
        create_info.height,
        create_info.vsync,
        create_info.fullscreen,
        create_info.borderless,
      });
    }

    ~Impl() {
      instantiated_ = false;
    }

    void add_global_data() {

    }

    void add_step_before() {

    }

    void add_system_to_step() {

    }

    void run() {
      game_loop();
    }

  private:
    static inline bool instantiated_{ false };

    bool running_{ true };
    Log dummy_log_; // this just allows for logging upon full destruction of App
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

    void game_loop() {
      while (window_->running()) {
        window_->poll_events();
      }
    }
  };

  //
  //  App
  //

  App::App(const foxy::AppCreateInfo&& create_info)
    : pImpl_{std::make_unique<Impl>(create_info)} {}

  App::~App() = default;

  void App::run() {
    pImpl_->run();
  }

  auto App::add_global_data() -> App& {
    pImpl_->add_global_data();
    return *this;
  }

  auto App::add_step_before() -> App& {
    pImpl_->add_step_before();
    return *this;
  }

  auto App::add_system_to_step() -> App& {
    pImpl_->add_system_to_step();
    return *this;
  }
}