//
// Created by galex on 9/6/2022.
//

#include "glfw.hpp"

namespace foxy {
  auto glfw::required_instance_extensions() -> std::vector<std::string> {
    std::vector<std::string> result;
    u32 count = 0;
    const char** names = glfwGetRequiredInstanceExtensions(&count);
    if (names && count) {
      for (u32 i{ 0 }; i < count; ++i) {
        result.emplace_back(names[i]);
      }
    }
    return result;
  }
}