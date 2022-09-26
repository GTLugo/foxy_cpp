#pragma once

namespace koyote {
  template<typename... Args>
  class Event {
  public:
    template<typename Callback>
    void add_callback(Callback&& callback) {
      handler_functions_.push_back(std::forward<Callback>(callback));
    }

    void operator()(Args... args) {
      for (auto&& function: handler_functions_) {
        function(args...);
      }
    }
  private:
    std::vector<std::function<void(Args...)>> handler_functions_;
  };
}