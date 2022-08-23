//
// Created by galex on 8/23/2022.
// Based entirely upon GLFWPP's event design, which is based upon GLFW's event design
//

#pragma once

namespace foxy {
  template<typename... Args>
  class Event {
  public:
    template<typename Callback>
    void set_callback(Callback&& callback_) {
      handler_function_ = std::forward<Callback>(callback_);
    }

    void operator()(Args... args_) {
      if (handler_function_) {
        handler_function_(args_...);
      }
    }
  private:
    std::function<void(Args...)> handler_function_;
  };
}