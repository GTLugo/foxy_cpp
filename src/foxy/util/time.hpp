
#pragma once
//
// Created by Gabriel Lugo on 3/27/2021.
// Flugel Framework: https://github.com/GTLugo/flugel_framework
//

#include <chrono>

namespace foxy {
  // duration types
  using NanoSeconds = std::chrono::duration<double, std::nano>;
  using MicroSeconds = std::chrono::duration<double, std::micro>;
  using MilliSeconds = std::chrono::duration<double, std::milli>;
  using Seconds = std::chrono::duration<double>;
  using Minutes = std::chrono::duration<double, std::ratio<60>>;
  using Hours = std::chrono::duration<double, std::ratio<3600>>;
  // clock types
  using ClockSystem = std::chrono::system_clock;
  using ClockSteady = std::chrono::steady_clock;
  using ClockAccurate = std::chrono::high_resolution_clock;
  // time point
  using TimePoint = ClockSteady::time_point;
  using TimePointAccurate = ClockAccurate::time_point;
  using TimePointSystem = ClockSystem::time_point;

  class Stopwatch {
  public:
    Stopwatch() {
      start();
    }

    explicit Stopwatch(TimePoint time_point) {
      start(time_point);
    }

    void start() { start(ClockSteady::now()); }

    void start(TimePoint timePoint) {
      start_ = timePoint;
    }

    template<class Duration>
    [[nodiscard]] double start_time() const {
      return Duration{(start_).time_since_epoch()}.count();
    }

    template<class Duration>
    [[nodiscard]] double get_time_elapsed() const {
      return Duration{(ClockSteady::now() - start_)}.count();
    }
  private:
    TimePoint start_{};
  };

  class Time {
  public:

    static void __foxy_internal_init(double tick_rate = 128., u32 bail_count = 1024U) {
      // This is awful and messy, but it'll prevent anyone outside the App class
      // from reinitializing Time, which would cause the engine, the app, life,
      // the universe, and all catgirls to die.
      if (!virgin_) return;
      FOXY_TRACE << "Initializing Time...";

      tick_rate_ = tick_rate;
      bail_count_ = bail_count;
      game_last_ = TimePoint{ClockSteady::now()};
      game_current_ = TimePoint{ClockSteady::now()};
      fixed_time_step_ = Seconds{1. / tick_rate_};
    }
//    explicit Time(double tickRate, uint32_t bailCount = 1024U)
//      : tickRate_{tickRate},
//        bailCount_{bailCount},
//        stopwatch_{ClockSteady::now()},
//        gameLast_{ClockSteady::now()},
//        gameCurrent_{ClockSteady::now()} {
//      fixedTimeStep_ = Seconds{1. / tickRate_};
//    }
//    ~Time() = default;

    [[nodiscard]] static double tick_rate() {
      return tick_rate_;
    }

    template<class Duration>
    [[nodiscard]] static double fixed_step() {
      return Duration{fixed_time_step_}.count();
    }

    template<class Duration>
    [[nodiscard]] static double start() {
      return stopwatch().start_time<Duration>();
    }

    template<class Duration>
    [[nodiscard]] static double since_start() {
      return stopwatch().get_time_elapsed<Duration>();
    }

    template<typename Duration>
    [[nodiscard]] static double now() {
      return Duration{ClockSteady::now().time_since_epoch()}.count();
    }

    template<class Duration>
    [[nodiscard]] static double delta() {
      return Duration{delta_}.count();
    }

    template<class Duration>
    [[nodiscard]] static double lag() {
      return Duration{lag_}.count();
    }
    
    // These should NEVER be called from anywhere other than the internal app class.

    static void __foxy_internal_update() {
      // FLUGEL_ENGINE_TRACE("Update!");
      game_current_ = ClockSteady::now();
      // Seconds::duration()
      delta_ = std::chrono::duration_cast<Seconds>(game_current_ - game_last_);
      game_last_ = game_current_;
      lag_ += delta_;
      step_count_ = 0U;
    }

    static void __foxy_internal_tick() {
      // FLUGEL_ENGINE_TRACE("Tick!");
      lag_ -= fixed_time_step_;
      ++step_count_;
    }

    [[nodiscard]] static bool __foxy_internal_should_do_tick() {
      if (step_count_ >= bail_count_) {
        FOXY_WARN << "Struggling to catch up with physics rate.";
      }

      return lag_.count() >= fixed_time_step_.count() && step_count_ < bail_count_;
    }
  private:
    static inline bool virgin_{true};
    // fixed number of ticks per second. this will be used for physics and anything else in fixed update
    static inline double tick_rate_{};
    static inline Seconds fixed_time_step_{};
    // bail out of the fixed updates if iterations exceeds this amount to prevent lockups
    // on extremely slow systems where updateFixed may be longer than fixedTimeStep_
    static inline u32 bail_count_{};

    static inline TimePoint game_last_{}; // when last frame started
    static inline TimePoint game_current_{}; // when this frame started
    static inline Seconds delta_{Seconds{1. / 60.}}; // how much time last frame took
    static inline Seconds lag_{Seconds::zero()}; // how far behind the game is from real world
    static inline u32 step_count_{0U};

    static const Stopwatch& stopwatch() {
      static const Stopwatch sw{ClockSteady::now()};
      return sw;
    };
  };
}