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
    explicit Impl(App& app, const CreateInfo& create_info) :
      app_{ app }
    {
      if (instantiated_) {
        Log::fatal("Attempted second instantiation of fx::App");
      }
      instantiated_ = true;
      Time::init(128, 1024U);

      window_ = std::make_unique<Window>(Window::CreateInfo{
        create_info.title,
        create_info.width,
        create_info.height,
        create_info.vsync,
        create_info.fullscreen,
        create_info.borderless
      });

      renderer_ = std::make_unique<RenderEngine>(**window_);

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

    void set_user_data(shared<void> data) {
      user_data_ = std::move(data);
    }

    void add_function_to_stage(const Stage stage, StageCallback&& callback)
    {
      stage_callback(stage).add_callback(std::forward<StageCallback>(callback));
    }

    [[nodiscard]] auto user_data() -> shared<void>
    {
      return user_data_;
    }

    void run()
    {
      thread_pool_.push_task(FOXY_LAMBDA(game_loop));
      main_loop();
      thread_pool_.wait_for_tasks();
    }

  private:
    static inline bool instantiated_{ false };

    const double frame_time_goal_{ 1. / 250. };

    shared<void> user_data_;
    App& app_;

    unique<Window> window_;
    unique<RenderEngine> renderer_;

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
    Event<App&> early_tick_event_;
    Event<App&> tick_event_;
    Event<App&> late_tick_event_;
    Event<App&> early_update_event_;
    Event<App&> update_event_;
    Event<App&> late_update_event_;
    Event<App&> stop_event_;
    Event<App&> asleep_event_;

    Event<App&> reserved_event_;

    void main_loop()
    {
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

    void awake(App& app)
    {

    }

    void start(App& app)
    {

    }

    void early_tick(App& app)
    {

    }

    void tick(App& app)
    {
    #if defined(FOXY_DEBUG_MODE) and defined(FOXY_PERF_TITLE)
      show_perf_stats();
    #endif
    }

    void late_tick(App& app)
    {

    }

    void early_update(App& app)
    {

    }

    void update(App& app)
    {

    }

    void late_update(App& app)
    {

    }

    void stop(App& app)
    {

    }

    void asleep(App& app)
    {

    }

    void set_callbacks()
    {
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

    void show_perf_stats()
    {
      static u32 counter{ 0 };
      const double frame_time{ Time::delta<secs>() };
      if (counter >= static_cast<u32>(Time::tick_rate()) / 2.) {
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

    [[nodiscard]] constexpr auto stage_callback(const Stage stage) -> Event<App&>&
    {
      switch (stage) {
      case Stage::Awake:       return awake_event_;
      case Stage::Start:       return start_event_;
      case Stage::EarlyTick:   return early_tick_event_;
      case Stage::Tick:        return tick_event_;
      case Stage::LateTick:    return late_tick_event_;
      case Stage::EarlyUpdate: return early_update_event_;
      case Stage::Update:      return update_event_;
      case Stage::LateUpdate:  return late_update_event_;
      case Stage::Stop:        return stop_event_;
      case Stage::Asleep:      return asleep_event_;
      }
      Log::fatal("Invalid stage value for callback.");
      return reserved_event_; // This just gets rid of the control path warning :)
    }
  };

  //
  //  App
  //

  App::App(CreateInfo&& create_info):
    p_impl_{ std::make_unique<Impl>(*this, create_info) } {}

  App::~App() = default;

  void App::run()
  {
    p_impl_->run();
  }

  auto App::set_user_data_ptr(shared<void> data) -> App& 
  {
    p_impl_->set_user_data(data);
    return *this;
  }

  auto App::add_function_to_stage(const Stage stage, StageCallback&& callback) -> App& 
  {
    p_impl_->add_function_to_stage(stage, std::forward<StageCallback>(callback));
    return *this;
  }

  auto App::user_data_ptr() -> shared<void>
  {
    return p_impl_->user_data();
  }
}