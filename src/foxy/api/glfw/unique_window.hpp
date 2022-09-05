//
// Created by galex on 9/5/2022.
//

#pragma once

class GLFWwindow;

namespace foxy::glfw {
  struct WindowDestructor {
    void operator()(GLFWwindow* ptr);
  };

  using UniqueWindow = Unique<GLFWwindow, WindowDestructor>;
} // foxy
