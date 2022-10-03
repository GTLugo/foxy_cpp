
#pragma once
//
// Created by Gabriel Lugo on 3/27/2021.
// Flugel Framework: https://github.com/GTLugo/flugel_framework
//

#include "koyote/core/std.hpp"
#include "koyote/core/log.hpp"

namespace fx {
  // duration types
  using nanosecs = std::chrono::duration<double, std::nano>;
  using microsecs = std::chrono::duration<double, std::micro>;
  using millisecs = std::chrono::duration<double, std::milli>;
  using secs = std::chrono::duration<double>;
  using mins = std::chrono::duration<double, std::ratio<60>>;
  using hours = std::chrono::duration<double, std::ratio<3600>>;
  // clock types
  using clock_system = std::chrono::system_clock;
  using clock_steady = std::chrono::steady_clock;
  using clock_accurate = std::chrono::high_resolution_clock;
  // time point
  using time_point = clock_steady::time_point;
  using time_point_a = clock_accurate::time_point;
  using time_point_s = clock_system::time_point;

  class Stopwatch {
  public:
    Stopwatch() {
      start();
    }

    explicit Stopwatch(time_point time_point) {
      start(time_point);
    }

    void start() { start(clock_steady::now()); }

    void start(time_point timePoint) {
      start_ = timePoint;
    }

    template<class Duration>
    [[nodiscard]] double start_time() const {
      return Duration{(start_).time_since_epoch()}.count();
    }

    template<class Duration>
    [[nodiscard]] double get_time_elapsed() const {
      return Duration{(clock_steady::now() - start_)}.count();
    }
  private:
    time_point start_{};
  }; // Stopwatch

  class Time {
    friend class GameLoop;
  public:
    Time(double tick_rate = 128., u32 bail_count = 1024U);
    ~Time() = default;

    [[nodiscard]] double tick_rate() const {
      return tick_rate_;
    }

    template<class Duration>
    [[nodiscard]] double fixed_step() const {
      return Duration{fixed_time_step_}.count();
    }

    template<typename Duration>
    [[nodiscard]] double now() const {
      return Duration{clock_steady::now().time_since_epoch()}.count();
    }

    template<class Duration>
    [[nodiscard]] double delta() const {
      return Duration{delta_}.count();
    }

    // [[nodiscard]] static double avg_delta_secs() {
    //   return std::accumulate(last_few_frame_times_.begin(), last_few_frame_times_.end(), 0.) / last_few_frame_times_.size();
    // }

    template<class Duration>
    [[nodiscard]] double lag() const {
      return Duration{lag_}.count();
    }
    
    // These should NEVER be called from anywhere other than the internal app class.

    [[nodiscard]] bool should_do_tick() const {
      if (step_count_ >= bail_count_) {
        fx::Log::warn("Struggling to catch up with physics rate.");
      }

      return lag_.count() >= fixed_time_step_.count() && step_count_ < bail_count_;
    }
  private:
    // fixed number of ticks per second. this will be used for physics and anything else in fixed update
    double tick_rate_{};
    secs fixed_time_step_{};
    // bail out of the fixed updates if iterations exceeds this amount to prevent lockups
    // on extremely slow systems where tick time may exceed fixed time step
    u32 bail_count_{};

    time_point game_last_{}; // when last frame started
    time_point game_current_{}; // when this frame started
    secs delta_{secs{1. / 60.}}; // how much time last frame took
    secs lag_{secs::zero()}; // how far behind the game is from real world
    u32 step_count_{0U};

    //static inline std::deque<double> last_few_frame_times_{};

    void internal_update() {
      // FLUGEL_ENGINE_TRACE("Update!");
      game_current_ = clock_steady::now();
      // Seconds::duration()
      delta_ = std::chrono::duration_cast<secs>(game_current_ - game_last_);
      // if (static_cast<u32>(last_few_frame_times_.size()) >= 10230U) {
      //   last_few_frame_times_.pop_front();
      // }
      // last_few_frame_times_.push_back(delta_.count());
      game_last_ = game_current_;
      lag_ += delta_;
      step_count_ = 0U;
    }

    void internal_tick() {
      // FLUGEL_ENGINE_TRACE("Tick!");
      lag_ -= fixed_time_step_;
      ++step_count_;
    }
  }; // Time
  
  class GameLoop {
  public:
    struct CreateInfo {
      const double tick_rate{ 128. };
      const u32 bail_count{ 1024U };
      const bool& stop_flag{ false };
      std::function<void(const Time&)> start{ [](const Time&){} };
      std::function<void(const Time&)> tick{ [](const Time&){} };
      std::function<void(const Time&)> update{ [](const Time&){} };
      std::function<void(const Time&)> stop{ [](const Time&){} };
    };
    
    explicit GameLoop(const CreateInfo& game_loop);
    ~GameLoop() = default;
  
    auto operator()() -> GameLoop;
    
    void run();
    
  private:
    Time time;
    const bool& stop_flag;
    std::function<void(const Time&)> start_callback;
    std::function<void(const Time&)> tick_callback;
    std::function<void(const Time&)> update_callback;
    std::function<void(const Time&)> stop_callback;
  }; // GameLoop
} // fx