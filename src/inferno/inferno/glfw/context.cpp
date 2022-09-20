//
// Created by galex on 9/3/2022.
//

#include "context.hpp"

// this is here to expose vulkan functions to glfw
#define FOXY_GLFW_INCLUDE_VULKAN
#include "glfw.hpp"

namespace inferno {
  inline static void glfw_error_callback(int error, const char* message) {
    koyote::Log::error("GFLW: {} | {}", error, message);
  }

  Context::Context() {
    if (instantiated_) {
      koyote::Log::fatal("Attempted second instantiation of foxy::glfw::Context");
    }
    instantiated_ = true;

    koyote::i32 glfw_init_success = glfwInit();
    if (!glfw_init_success) {
      koyote::Log::fatal("Failed to initialize GLFW!");
    }
    koyote::i32 major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);
    glfwSetErrorCallback(glfw_error_callback);

    koyote::Log::debug("GLFW Version {}.{}.{}", major, minor, revision);
  }

  Context::~Context() {
    instantiated_ = false;
    glfwTerminate();
  }
}