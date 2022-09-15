#include "app.hpp"

#include "foxy/util/time.hpp"
#include "foxy/core/window.hpp"
#include "foxy/core/events/event.hpp"
#include "foxy/ookami/renderer.hpp"
#include <BS_thread_pool.hpp>
//#include "foxy/koyote/ecs.hpp"

namespace foxy {
  class App::Impl {
    friend class App;
  public:
    explicit Impl(App& app, const CreateInfo& create_info)
      : dummy_log_{},
        app_{app} {
      FOXY_ASSERT(!instantiated_) << "Attempted second instantiation of foxy::App";
      instantiated_ = true;

      Time::__foxy_internal_init(128);

      window_ = std::make_unique<Window>(Window::CreateInfo{
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

    void set_user_data(Shared<void> data) {
      user_data_ = data;
    }

    void add_stage_before() {

    }
    
    void add_function_to_stage(Stage stage, StageCallback&& callback) {
      switch (stage) {
        case Stage::Awake:
          awake_event_.add_callback(std::forward<StageCallback>(callback));
          break;
        case Stage::Start:
          start_event_.add_callback(std::forward<StageCallback>(callback));
          break;
        case Stage::EarlyUpdate:
          early_update_event_.add_callback(std::forward<StageCallback>(callback));
          break;
        case Stage::Tick:
          tick_event_.add_callback(std::forward<StageCallback>(callback));
          break;
        case Stage::LateUpdate:
          late_update_event_.add_callback(std::forward<StageCallback>(callback));
          break;
        case Stage::Stop:
          stop_event_.add_callback(std::forward<StageCallback>(callback));
          break;
      }
    }

    [[nodiscard]] auto user_data() -> Shared<void> {
      return user_data_;
    }

    void run() {
      thread_pool_.push_task(FOXY_LAMBDA(game_loop));
      main_loop();
      thread_pool_.wait_for_tasks();
    }

  private:
    static inline bool instantiated_{ false };
    Log dummy_log_; // this just allows for logging upon first creation and final destruction of App

    const double frame_time_goal_{ 1. / 250. };
    bool running_{ true };

    Shared<void> user_data_;
    App& app_;

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
    Event<App&> awake_event_;
    Event<App&> start_event_;
    Event<App&> early_update_event_;
    Event<App&> tick_event_;
    Event<App&> late_update_event_;
    Event<App&> stop_event_;

    void main_loop() {
      while (window_->running()) {
        main_poll_event_();
      }
    }

    void game_loop() {
      el::Helpers::setThreadName("game");
      FOXY_TRACE << "Starting game thread...";
      try {
        awake_event_(app_);
        start_event_(app_);
        while (window_->running()) {
          early_update_event_(app_);
          while (Time::__foxy_internal_should_do_tick()) {
            tick_event_(app_);
            Time::__foxy_internal_tick();
          }
          late_update_event_(app_);
          Time::__foxy_internal_update();
        }
        stop_event_(app_);
      } catch (const std::exception& e) {
        FOXY_ERROR << e.what();
      }

      FOXY_TRACE << "Joining game thread...";
    }

    void awake(App& app) {
      
    }

    void start(App& app) {
      //FOXY_DEBUG << "Start";
    }

    void early_update(App& app) {
      //FOXY_DEBUG << "Early";
    }

    void tick(App& app) {
      #if defined(FOXY_DEBUG_MODE) and defined(FOXY_PERF_TITLE)
      show_perf_stats();
      #endif
    }

    void late_update(App& app) {
      //FOXY_DEBUG << "Late";
    }

    void stop(App& app) {
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

    void show_perf_stats() {
      static u32 counter{ 0 };
      double frame_time{ Time::delta<Seconds>() };
      if (counter >= static_cast<u32>(Time::tick_rate())) {
        std::stringstream perf_stats;

        perf_stats << "frametime: " 
          << std::fixed << std::setfill(' ') << std::setw(12) << std::setprecision(9) << frame_time << "s | % of target frametime ceiling: " 
          << std::defaultfloat << std::setfill(' ') << std::setw(9) << std::setprecision(4) << (frame_time / frame_time_goal_) * 100. << '%';

        //FOXY_DEBUG << "PERF STATS | " << perf_stats.str();
        window_->set_subtitle(perf_stats.str());
        counter = 0;
      } else {
        ++counter;
      }
    }
  };

  //
  //  App
  //

  App::App(App::CreateInfo&& create_info)
    : pImpl_{std::make_unique<Impl>(*this, create_info)} {}

  App::~App() = default;

  void App::run() {
    pImpl_->run();
  }

  auto App::set_user_data(Shared<void> data) -> App& {
    pImpl_->set_user_data(data);
    return *this;
  }

  auto App::add_stage_before() -> App& {
    pImpl_->add_stage_before();
    return *this;
  }

  auto App::add_function_to_stage(Stage stage, StageCallback&& callback) -> App& {
    pImpl_->add_function_to_stage(stage, std::forward<StageCallback>(callback));
    return *this;
  }

  auto App::user_data() -> Shared<void> {
    return pImpl_->user_data();
  }
}