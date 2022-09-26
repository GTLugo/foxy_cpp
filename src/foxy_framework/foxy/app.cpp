#include "app.hpp"

#include <utility>

#include "inferno/window.hpp"
#include "ookami/render_engine.hpp"
#include "inu/job_system.hpp"
#include "neko/ecs.hpp"

namespace fx {
  class App::Impl {
    friend class App;
  public:
    explicit Impl(App& app, const CreateInfo& create_info)
      : app_{app} {
      if (instantiated_) {
        fx::Log::fatal("Attempted second instantiation of fx::App");
      }
      instantiated_ = true;
      fx::Time::init(128, 1024U);

      window_ = std::make_unique<fx::Window>(fx::Window::CreateInfo{
        create_info.title,
        create_info.width,
        create_info.height,
        create_info.vsync,
        create_info.fullscreen,
        create_info.borderless,
      });

      renderer_ = std::make_unique<fx::RenderEngine>(**window_);

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

    void set_user_data(fx::shared<void> data) {
      user_data_ = std::move(data);
    }

    void add_stage_before() {

    }
    
    void add_function_to_stage(const Stage stage, StageCallback&& callback) {
      switch (stage) {
        case Stage::Awake:
          awake_event_.add_callback(std::forward<StageCallback>(callback));
          break;
        case Stage::Start:
          start_event_.add_callback(std::forward<StageCallback>(callback));
          break;
        case Stage::EarlyTick:
          tick_event_.add_callback(std::forward<StageCallback>(callback));
          break;
        case Stage::Tick:
          tick_event_.add_callback(std::forward<StageCallback>(callback));
          break;
        case Stage::LateTick:
          tick_event_.add_callback(std::forward<StageCallback>(callback));
          break;
        case Stage::EarlyUpdate:
          early_update_event_.add_callback(std::forward<StageCallback>(callback));
          break;
        case Stage::Update:
          early_update_event_.add_callback(std::forward<StageCallback>(callback));
          break;
        case Stage::LateUpdate:
          late_update_event_.add_callback(std::forward<StageCallback>(callback));
          break;
        case Stage::Stop:
          stop_event_.add_callback(std::forward<StageCallback>(callback));
          break;
        case Stage::Asleep:
          stop_event_.add_callback(std::forward<StageCallback>(callback));
          break;
      }
    }

    [[nodiscard]] auto user_data() -> fx::shared<void> {
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

    fx::shared<void> user_data_;
    App& app_;

    fx::unique<fx::Window> window_;
    fx::unique<fx::RenderEngine> renderer_;

    BS::thread_pool thread_pool_{ std::thread::hardware_concurrency() - 1 };

    // Main Thread events
    fx::Event<> main_awake_event_;
    fx::Event<> main_start_event_;
    fx::Event<> main_poll_event_;
    fx::Event<> main_update_event_;
    fx::Event<> main_stop_event_;
    // Game Thread events
    fx::Event<App&> awake_event_;
    fx::Event<App&> start_event_;
    fx::Event<App&> early_tick_event_;
    fx::Event<App&> tick_event_;
    fx::Event<App&> late_tick_event_;
    fx::Event<App&> early_update_event_;
    fx::Event<App&> update_event_;
    fx::Event<App&> late_update_event_;
    fx::Event<App&> stop_event_;
    fx::Event<App&> asleep_event_;

    void main_loop() {
      main_awake_event_();
      main_start_event_();
      while (!window_->should_stop()) {
        main_poll_event_();
        main_update_event_();
      }
      main_stop_event_();
    }

    void game_loop()
    {
      Log::set_thread_name("game");
      Log::trace("Starting game thread...");
      try {
        awake_event_(app_);
        start_event_(app_);
        Time::internal_game_loop(
          window_->should_stop(),
          [this] { // Tick
            early_tick_event_(app_);
            tick_event_(app_);
            late_tick_event_(app_);
          },
          [this] { // Update
            early_update_event_(app_);
            update_event_(app_);
            late_update_event_(app_);
          });
        stop_event_(app_);
        asleep_event_(app_);
      } catch (const std::exception& e) {
        Log::error(e.what());
      }
      Log::trace("Joining game thread...");
    }

    void awake(App& app) {
      
    }

    void start(App& app) {
      //FOXY_DEBUG << "Start";
    }

    void early_tick(App& app)
    {

    }

    void tick(App& app) {
    #if defined(FOXY_DEBUG_MODE) and defined(FOXY_PERF_TITLE)
      show_perf_stats();
    #endif
    }

    void late_tick(App& app)
    {

    }

    void early_update(App& app) {
      //FOXY_DEBUG << "Early";
    }

    void update(App& app)
    {

    }

    void late_update(App& app) {
      //FOXY_DEBUG << "Late";
    }

    void stop(App& app) {
      // fx::Log::info("Stop");
    }

    void asleep(App& app)
    {
      // fx::Log::info("Stop");
    }

    void set_callbacks() {
      main_poll_event_.add_callback(FOXY_LAMBDA(window_->poll_events));

      awake_event_.add_callback(FOXY_LAMBDA(awake));
      start_event_.add_callback(FOXY_LAMBDA(start));

      early_tick_event_.add_callback(FOXY_LAMBDA(early_tick));
      tick_event_.add_callback(FOXY_LAMBDA(tick));
      late_tick_event_.add_callback(FOXY_LAMBDA(late_tick));

      early_update_event_.add_callback(FOXY_LAMBDA(early_update));
      update_event_.add_callback(FOXY_LAMBDA(update));
      late_update_event_.add_callback(FOXY_LAMBDA(late_update));

      stop_event_.add_callback(FOXY_LAMBDA(stop));
      asleep_event_.add_callback(FOXY_LAMBDA(asleep));
    }

    void show_perf_stats() {
      static fx::u32 counter{ 0 };
      const double frame_time{ fx::Time::delta<fx::secs>() };
      if (counter >= static_cast<fx::u32>(fx::Time::tick_rate()) / 2.) {
        std::stringstream perf_stats;

        perf_stats << "frametime: " 
          << std::fixed << std::setfill(' ') << std::setw(12) << std::setprecision(9) << frame_time << "s | % of target frametime ceiling: " 
          << std::defaultfloat << std::setfill(' ') << std::setw(9) << std::setprecision(4) << (frame_time / frame_time_goal_) * 100. << '%';

        // FOXY_DEBUG << "PERF STATS | " << perf_stats.str();
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
    : p_impl_{std::make_unique<Impl>(*this, create_info)} {}

  App::~App() = default;

  void App::run() {
    p_impl_->run();
  }

  auto App::set_user_data_ptr(fx::shared<void> data) -> App& {
    p_impl_->set_user_data(data);
    return *this;
  }

  auto App::add_stage_before() -> App& {
    p_impl_->add_stage_before();
    return *this;
  }

  auto App::add_function_to_stage(const Stage stage, StageCallback&& callback) -> App& {
    p_impl_->add_function_to_stage(stage, std::forward<StageCallback>(callback));
    return *this;
  }

  auto App::user_data_ptr() -> fx::shared<void> {
    return p_impl_->user_data();
  }
}