//
// Created by galex on 9/3/2022.
//

#pragma once

namespace foxy::glfw {
  class Context {
  public:
    Context();
    ~Context();

  private:
    static inline bool instantiated_{ false };
  };
}
