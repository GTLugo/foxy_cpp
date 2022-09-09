//
// Created by galex on 9/3/2022.
//

#include "context.hpp"

// this is here to expose vulkan functions to glfw
#define FOXY_GLFW_INCLUDE_VULKAN
#include "foxy/core/window/glfw/glfw.hpp"

namespace foxy::glfw {
  inline static void glfw_error_callback(int error, const char* message) {
    FOXY_ERROR << "GFLW: " << error << " | " << message;
  }

  Context::Context() {
    FOXY_ASSERT(!instantiated_) << "Attempted second instantiation of foxy::glfw::Context";
    instantiated_ = true;

    i32 glfwInitSuccess = glfwInit();
    DCHECK(glfwInitSuccess) << "Failed to initialize GLFW!";
    i32 major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);
    glfwSetErrorCallback(glfw_error_callback);

    FOXY_INFO << "GLFW Version " << major << "." << minor << "." << revision;
  }

  Context::~Context() {
    instantiated_ = false;
    glfwTerminate();
  }
}