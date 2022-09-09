//
// Created by galex on 9/1/2022.
//

#pragma once


#define GLFW_INCLUDE_NONE
#ifdef FOXY_GLFW_INCLUDE_VULKAN
#include "foxy/ookami/vulkan/vulkan.hpp"
#endif
#include <GLFW/glfw3.h>
//#include "unique_window.hpp"

namespace foxy::glfw {
  auto required_instance_extensions_strings() -> std::vector<std::string>;
  auto required_instance_extensions() -> std::vector<const char*>;
}
