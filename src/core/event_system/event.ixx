module;

#include "foxy/internal/foxy_includes.hpp"

#ifdef __INTELLISENSE__
#include "util/util.ixx"
#endif

export module foxy_events;

#ifndef __INTELLISENSE__
export import foxy_util;
#endif

namespace foxy {
  export template<typename... Args>
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