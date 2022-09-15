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

      Time::__foxy_internal_init(128);

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
    const double frame_time_goal_{ 1. / 250. };

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
    Event<> awake_event_;
    Event<> start_event_;
    Event<double> early_update_event_;
    Event<double> tick_event_;
    Event<double> late_update_event_;
    Event<> stop_event_;

    void main_loop() {
      while (window_->running()) {
        main_poll_event_();
      }
    }

    void game_loop() {
      el::Helpers::setThreadName("game");
      FOXY_TRACE << "Starting game thread...";

      try {
        awake_event_();
        start_event_();
        while (window_->running()) {
          double frame_time{ Time::delta<Seconds>() };
          early_update_event_(frame_time);
          while (Time::__foxy_internal_should_do_tick()) {
            tick_event_(frame_time);
            Time::__foxy_internal_tick();
          }
          late_update_event_(frame_time);
          Time::__foxy_internal_update();
        }
        stop_event_();
      } catch (const std::exception& e) {
        FOXY_ERROR << e.what();
      }

      FOXY_TRACE << "Joining game thread...";
    }

    void awake() {

    }

    void start() {
      //FOXY_DEBUG << "Start";
    }

    void early_update(double frame_time) {
      //FOXY_DEBUG << "Early";
    }

    void tick(double frame_time) {
      LOG_EVERY_N(static_cast<u32>(Time::tick_rate()), DEBUG) << "PERF STATS | frame time: " 
        << std::fixed << std::setfill(' ') << std::setw(12) << std::setprecision(9) << frame_time << "s | % of cap: " 
        << std::defaultfloat << std::setfill(' ') << std::setw(9) << std::setprecision(4) << (frame_time / frame_time_goal_) * 100. << '%';
    }

    void late_update(double frame_time) {
      //FOXY_DEBUG << "Late";
    }

    void stop() {
      //FOXY_DEBUG << "Stop";
    }

    void set_callbacks() {
      main_poll_event_.add_callback(FOXY_LAMBDA(window_->poll_events));

      awake_event_.add_callback(FOXY_LAMBDA(awake));
      start_event_.add_callback(FOXY_LAMBDA(start));
      early_update_event_.add_callback(FOXY_LAMBDA(early_update));
      tick_event_.add_callback(FOXY_LAMBDA(tick));
      late_update_event_.add_callback(FOXY_LAMBDA(late_update));
      stop_event_.add_callback(FOXY_LAMBDA(stop));
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