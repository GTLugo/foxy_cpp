//
// Created by galex on 9/5/2022.
//

#include "unique_window.hpp"

#include "foxy/api/glfw/glfw.hpp"

namespace foxy {
  void glfw::WindowDestructor::operator()(GLFWwindow* ptr) {
    glfwDestroyWindow(ptr);
  }
} // foxy