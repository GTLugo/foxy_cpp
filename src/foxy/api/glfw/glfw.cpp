//
// Created by galex on 9/6/2022.
//

#include "glfw.hpp"

namespace foxy {
  auto glfw::required_instance_extensions_strings() -> std::vector<std::string> {
    std::vector<std::string> result;
    auto count = u32{ 0 };
    const char** extensions = glfwGetRequiredInstanceExtensions(&count);
    for (auto i = u32{ 0 }; i < count; ++i) {
      result.emplace_back(extensions[i]);
    }
    return result;
  }

  auto glfw::required_instance_extensions() -> std::vector<const char*> {
    auto count = u32{ 0 };
    const char** extensions = glfwGetRequiredInstanceExtensions(&count);
    std::vector<const char*> result{extensions, extensions + count};

//    for (auto i = u32{ 0 }; i < count; ++i) {
//      result.emplace_back(extensions[i]);
//    }
    return result;
  }
}