//
// Created by galex on 9/3/2022.
//

#pragma once

#include "glfw.hpp"

namespace fx::inferno {
  struct WindowCreateInfo {
    const std::string title{ "FOXY FRAMEWORK" };
    const int width{ 1600 };
    const int height{ 900 };
  };

  [[nodiscard]] inline auto create_window(const WindowCreateInfo& create_info) -> fx::unique<GLFWwindow, void(*)(GLFWwindow*)> {
    return fx::unique<GLFWwindow, void(*)(GLFWwindow*)>{
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

  class Context: types::SingleInstance<Context> {
  public:
    Context();
    ~Context();
  };
}
