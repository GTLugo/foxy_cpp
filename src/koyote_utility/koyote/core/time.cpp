#include "time.hpp"

namespace fx {
  GameLoop::GameLoop(const GameLoop::CreateInfo& game_loop):
    time{ game_loop.tick_rate, game_loop.bail_count },
    stop_flag{ game_loop.stop_flag },
    start_callback{ game_loop.start },
    tick_callback{ game_loop.tick },
    update_callback{ game_loop.update },
    stop_callback{ game_loop.stop } {}
  
  auto GameLoop::operator()() -> GameLoop
  {
    run();
    return *this;
  }
  
  void GameLoop::run()
  {
    start_callback(time);
    while (!stop_flag) {
      while (time.should_do_tick()) {
        time.internal_tick();
      }
      update_callback(time);
      time.internal_update();
    }
    stop_callback(time);
  }
} // fx