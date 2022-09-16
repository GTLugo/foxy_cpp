//
// Created by galex on 9/1/2022.
//

#pragma once

#define GLFW_INCLUDE_NONE
#include <vulkan/vulkan_raii.hpp>
#include <GLFW/glfw3.h>

namespace ifr {
  auto required_instance_extensions_strings() -> std::vector<std::string>;
  auto required_instance_extensions() -> std::vector<const char*>;
}
