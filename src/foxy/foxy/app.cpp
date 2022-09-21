#include "app.hpp"

#include "inferno/window.hpp"
#include "inferno/event/event.hpp"
#include "ookami/render_engine.hpp"
#include "inu/job_system.hpp"
#include "neko/ecs.hpp"

namespace foxy {
  class App::Impl {
    friend class App;
  public:
    explicit Impl(App& app, const CreateInfo& create_info)
      : app_{app} {
      if (instantiated_) {
        koyote::Log::fatal("Attempted second instantiation of foxy::App");
      }
      instantiated_ = true;
      koyote::Time::i__engine_internal_init(128, 1024U);

      window_ = std::make_unique<inferno::Window>(inferno::Window::CreateInfo{
        create_info.title,
        create_info.width,
        create_info.height,
        create_info.vsync,
        create_info.fullscreen,
        create_info.borderless,
      });

      renderer_ = std::make_unique<ookami::RenderEngine>(**window_);

      window_->set_hidden(false);
      
      set_callbacks();
    }

    // Foxy Framework
    // Ookami Render Engine
    // Neko ECS/Inu Job System
    // Koyote Utilities

    ~Impl() {
      instantiated_ = false;
    }

    void set_user_data(koyote::shared<void> data) {
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

    [[nodiscard]] auto user_data() -> koyote::shared<void> {
      return user_data_;
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

    koyote::shared<void> user_data_;
    App& app_;

    koyote::unique<inferno::Window> window_;
    koyote::unique<ookami::RenderEngine> renderer_;

    BS::thread_pool thread_pool_{ std::thread::hardware_concurrency() - 1 };

    // Main Thread events
    inferno::Event<> main_awake_event_;
    inferno::Event<> main_start_event_;
    inferno::Event<> main_poll_event_;
    inferno::Event<> main_update_event_;
    inferno::Event<> main_stop_event_;
    // Game Thread events
    inferno::Event<App&> awake_event_;
    inferno::Event<App&> start_event_;
    inferno::Event<App&> early_update_event_;
    inferno::Event<App&> tick_event_;
    inferno::Event<App&> late_update_event_;
    inferno::Event<App&> stop_event_;

    void main_loop() {
      while (window_->running()) {
        main_poll_event_();
      }
    }

    void game_loop() {
      koyote::Log::set_thread_name("game");

      koyote::Log::trace("Starting game thread...");
      try {
        awake_event_(app_);
        start_event_(app_);
        while (window_->running()) {
          early_update_event_(app_);
          while (koyote::Time::i__engine_internal_should_do_tick()) {
            tick_event_(app_);
            koyote::Time::i__engine_internal_tick();
          }
          late_update_event_(app_);
          koyote::Time::i__engine_internal_update();
        }
        stop_event_(app_);
      } catch (const std::exception& e) {
        koyote::Log::error("{}", e.what());
      }

      koyote::Log::trace("Joining game thread...");
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
      static koyote::u32 counter{ 0 };
      double frame_time{ koyote::Time::delta<koyote::secs>() };
      if (counter >= static_cast<koyote::u32>(koyote::Time::tick_rate()) / 2.) {
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

  auto App::set_user_data_ptr(koyote::shared<void> data) -> App& {
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

  auto App::user_data_ptr() -> koyote::shared<void> {
    return pImpl_->user_data();
  }
}