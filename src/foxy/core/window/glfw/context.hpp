//
// Created by galex on 9/3/2022.
//

#pragma once

#include "foxy/core/window/glfw/glfw.hpp"

namespace foxy::glfw {
//  struct WindowDestructor {
//    void operator()(GLFWwindow* ptr) {
//      glfwDestroyWindow(ptr);
//    }
//  };
  using UniqueWindow = Unique<GLFWwindow, void(*)(GLFWwindow*)>;

  struct WindowCreateInfo {
    const std::string title{ "FOXY FRAMEWORK" };
    const int width{ 1600 };
    const int height{ 900 };
  };

  [[nodiscard]] inline auto create_window(const glfw::WindowCreateInfo& create_info) -> UniqueWindow {
    return UniqueWindow{
      glfwCreateWindow(
        create_info.width,
        create_info.height,
        create_info.title.c_str(),
        nullptr,
        nullptr
      ),
      [](GLFWwindow* window) {
        glfwDestroyWindow(window);
      }
    };
  }

  class Context {
  public:
    Context();
    ~Context();

  private:
    static inline bool instantiated_{ false };
  };
}
