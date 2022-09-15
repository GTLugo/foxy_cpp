#pragma once

namespace foxy {
  template<typename... Args>
  class Event {
  public:
    template<typename Callback>
    void add_callback(Callback&& callback_) {
      handler_functions_.push_back(std::forward<Callback>(callback_));
    }

    void operator()(Args... args_) {
      for (auto&& function: handler_functions_) {
        function(args_...);
      }
    }
  private:
    std::vector<std::function<void(Args...)>> handler_functions_;
  };
}