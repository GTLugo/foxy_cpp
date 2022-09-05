//
// Created by galex on 9/1/2022.
//

#pragma once

#include "foxy/api/glfw/unique_window.hpp"

#define GLFW_INCLUDE_NONE
#ifdef FOXY_GLFW_INCLUDE_VULKAN
#include "foxy/api/vulkan/vulkan.hpp"
#endif
#include <GLFW/glfw3.h>
//#define VKFW_NO_STRUCT_CONSTRUCTORS
//#define VKFW_NO_LEADING_e_IN_ENUMS
//#define VKFW_NO_INCLUDE_VULKAN
//#define VKFW_NO_INCLUDE_VULKAN_HPP
//#include <vkfw/vkfw.hpp>

//namespace foxy::glfw {
//  struct WindowDestructor {
//    void operator()(GLFWwindow* ptr) {
//      glfwDestroyWindow(ptr);
//    }
//  };
//
//  using UniqueWindow = Unique<GLFWwindow, WindowDestructor>;
//}
