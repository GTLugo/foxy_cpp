#include "app.hpp"

#include <utility>

#include <foxy/foxy_config.hpp>
#include <inferno/window.hpp>
#include <ookami/render_engine.hpp>
#include <inu/job_system.hpp>
#include <neko/ecs.hpp>

namespace fx {
  struct AppLoggingHelper {
    AppLoggingHelper()
    {
      Log::info(R"(--------------=============[])");
      Log::info(R"(  ______ ______   ___     __ )");
      Log::info(R"( |  ____/ __ \ \ / \ \   / / )");
      Log::info(R"( | |__ | |  | \ V / \ \_/ /  )");
      Log::info(R"( |  __|| |  | |> <   \   /   )");
      Log::info(R"( | |   | |__| / . \   | |    )");
      Log::info(R"( |_|    \____/_/ \_\  |_|    )");
      Log::info(R"(                             )");
      Log::info(R"(--------------=============[])");
      Log::info("Foxy startup: Kon kon kitsune! Hi, friends!");
      std::string build_mode{
      #if defined(FOXY_DEBUG_MODE) and not defined(FOXY_RELDEB_MODE)
        "DEBUG"
      #else
        "RELEASE"
      #endif
      };
      Log::info("Version: {}.{} : Build mode: DEBUG", FOXY_VERSION_MAJOR, FOXY_VERSION_MINOR, build_mode);
    }
    
    ~AppLoggingHelper()
    {
      #if defined(FOXY_DEBUG_MODE) or defined(FOXY_RELDEB_MODE)
      Log::info("Foxy shutdown: Otsukon deshita! Bye bye!");
      #endif
    }
  };
  
  class App::Impl: types::SingleInstance<App>, AppLoggingHelper {
    friend class App;
  
  public:
    explicit Impl(App& app, const CreateInfo& create_info):
      app_{ app },
      window_{
        std::make_unique<Window>(
          Window::CreateInfo{
            .title = create_info.title,
            .width = create_info.width,
            .height = create_info.height,
            .vsync = create_info.vsync,
            .fullscreen = create_info.fullscreen,
            .borderless = create_info.borderless
          }
        )
      },
      render_engine_{ std::make_unique<RenderEngine>(**window_) }
    {
      window_->set_hidden(false);
      set_callbacks();
    }
    
    ~Impl() = default;
    
    void set_user_data(shared<void> data)
    {
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
    const double frame_time_goal_{ 1. / 250. };
    
    shared<void> user_data_;
    App& app_;
    
    unique<Window> window_;
    unique<RenderEngine> render_engine_;
    
    BS::thread_pool thread_pool_{ std::thread::hardware_concurrency() - 1 };
    
    // Main Thread events
    Event<const Time&> main_awake_event_;
    Event<const Time&> main_start_event_;
    Event<const Time&> main_poll_event_;
    Event<const Time&> main_update_event_;
    Event<const Time&> main_stop_event_;
    // Game Thread events
    Event<App&, const Time&> awake_event_;
    Event<App&, const Time&> start_event_;
    Event<App&, const Time&> early_tick_event_;
    Event<App&, const Time&> tick_event_;
    Event<App&, const Time&> late_tick_event_;
    Event<App&, const Time&> early_update_event_;
    Event<App&, const Time&> update_event_;
    Event<App&, const Time&> late_update_event_;
    Event<App&, const Time&> stop_event_;
    Event<App&, const Time&> asleep_event_;
    
    Event<App&, const Time&> reserved_event_;
    
  private:
    void main_loop()
    {
      GameLoop{
        .start = [this](const Time& time) {
          main_awake_event_(time);
          main_start_event_(time);
        },
        .update = [this](const Time& time) { // Update
          main_poll_event_(time);
          main_update_event_(time);
        },
        .stop = [this](const Time& time) {
          main_stop_event_(time);
        }
      }(window_->should_stop());
    }
    
    void game_loop()
    {
      Log::set_thread_name("game");
      Log::trace("Starting game thread...");
      
      try {
        GameLoop{
          .start = [this](const Time& time) {
            awake_event_(app_, time);
            start_event_(app_, time);
          },
          .tick = [this](const Time& time) { // Tick
            early_tick_event_(app_, time);
            tick_event_(app_, time);
            late_tick_event_(app_, time);
          },
          .update = [this](const Time& time) { // Update
            early_update_event_(app_, time);
            update_event_(app_, time);
            late_update_event_(app_, time);
            render_engine_->draw_frame();
          },
          .stop = [this](const Time& time) {
            stop_event_(app_, time);
            asleep_event_(app_, time);
          }
        }(window_->should_stop());
      } catch (const std::exception& e) {
        Log::error(e.what());
      }
      
      Log::trace("Joining game thread into main thread...");
    }
    
    void awake(App& app, const Time& time)
    {
    
    }
    
    void start(App& app, const Time& time)
    {
    
    }
    
    void early_tick(App& app, const Time& time)
    {
    
    }
    
    void tick(App& app, const Time& time)
    {
      #if defined(FOXY_PERF_TITLE)
      show_perf_stats(time);
      #endif
    }
    
    void late_tick(App& app, const Time& time)
    {
    
    }
    
    void early_update(App& app, const Time& time)
    {
    
    }
    
    void update(App& app, const Time& time)
    {
    
    }
    
    void late_update(App& app, const Time& time)
    {
    
    }
    
    void stop(App& app, const Time& time)
    {
    
    }
    
    void asleep(App& app, const Time& time)
    {
    
    }
    
    void set_callbacks()
    {
      main_poll_event_.add_callback([this](const Time& time){ window_->poll_events(); });
      
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
    
    void show_perf_stats(const Time& time)
    {
      static u32 counter{ 0 };
      const double frame_time{ time.delta<secs>() };
      if (counter >= static_cast<u32>(time.tick_rate()) / 4.) {
        std::stringstream perf_stats;
        
        perf_stats << "frametime: "
                   << std::fixed << std::setfill(' ') << std::setw(12) << std::setprecision(9)
                   << frame_time << "s | % of target frametime ceiling: "
                   << std::defaultfloat << std::setfill(' ') << std::setw(9) << std::setprecision(4)
                   << (frame_time / frame_time_goal_) * 100. << '%';
        
        // Log::debug("PERF STATS | {}", perf_stats.str());
        window_->set_subtitle(perf_stats.str());
        counter = 0;
      } else {
        ++counter;
      }
    }
    
    [[nodiscard]] constexpr auto stage_callback(const Stage stage) -> Event<App&, const Time&>&
    {
      switch (stage) {
        case Stage::Awake:
          return awake_event_;
        case Stage::Start:
          return start_event_;
        case Stage::EarlyTick:
          return early_tick_event_;
        case Stage::Tick:
          return tick_event_;
        case Stage::LateTick:
          return late_tick_event_;
        case Stage::EarlyUpdate:
          return early_update_event_;
        case Stage::Update:
          return update_event_;
        case Stage::LateUpdate:
          return late_update_event_;
        case Stage::Stop:
          return stop_event_;
        case Stage::Asleep:
          return asleep_event_;
      }
      Log::fatal("Invalid stage value for callback.");
      return reserved_event_; // This just gets rid of the control path warning :)
    }
  };
  
  //
  //  App
  //
  
  App::App(CreateInfo&& create_info):
    p_impl_{ std::make_unique<Impl>(*this, create_info) }
  {}
  
  App::~App() = default;
  
  auto App::set_user_data_ptr(shared<void> data) -> App&
  {
    p_impl_->set_user_data(std::move(data));
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
  
  void App::run()
  {
    p_impl_->run();
  }
  
  auto App::operator()() -> void
  {
    return p_impl_->run();
  }
}