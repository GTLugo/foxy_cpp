#include "app.hpp"

#include "foxy/util/time.hpp"
#include "foxy/core/window.hpp"
#include "foxy/core/events/event.hpp"
#include "foxy/ookami/renderer.hpp"
#include <BS_thread_pool.hpp>
//#include "foxy/koyote/ecs.hpp"

namespace foxy {
  class App::Impl {
  public:
    explicit Impl(const foxy::AppCreateInfo& create_info)
      : dummy_log_{} {
      FOXY_ASSERT(!instantiated_) << "Attempted second instantiation of foxy::App";
      instantiated_ = true;

      Time::__foxy_internal_init(1);

      window_ = std::make_unique<Window>(WindowCreateInfo{
        create_info.title,
        create_info.width,
        create_info.height,
        create_info.vsync,
        create_info.fullscreen,
        create_info.borderless,
      });

      renderer_ = std::make_unique<ookami::Renderer>(**window_);

      window_->set_hidden(false);
      
      set_callbacks();
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
      thread_pool_.push_task(FOXY_LAMBDA(game_loop));
      main_loop();
      thread_pool_.wait_for_tasks();
    }

  private:
    static inline bool instantiated_{ false };

    bool running_{ true };
    Log dummy_log_; // this just allows for logging upon first creation and final destruction of App
    Unique<Window> window_;

    Unique<ookami::Renderer> renderer_;

    BS::thread_pool thread_pool_{ std::thread::hardware_concurrency() - 1 };

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

    void main_loop() {
      while (window_->running()) {
        main_poll_event_();
      }
    }

    void game_loop() {
      el::Helpers::setThreadName("game");
      FOXY_TRACE << "Starting game thread...";
      try {
        while (window_->running()) {
          double frame_time{ Time::delta<MilliSeconds>() };
          while (Time::__foxy_internal_should_do_tick()) {
            FOXY_DEBUG << "ft: " << std::setprecision(5) << frame_time << std::setprecision(5) << " fps: " << 1. / frame_time;
            Time::__foxy_internal_tick();
          }
          Time::__foxy_internal_update();
        }
      } catch (const std::exception& e) {
        FOXY_ERROR << e.what();
      }
      FOXY_TRACE << "Joining game thread...";
    }

    void set_callbacks() {
      // Foxy
      main_poll_event_.set_callback(FOXY_LAMBDA(window_->poll_events));
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