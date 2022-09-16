//
// Created by galex on 9/3/2022.
//

#include "context.hpp"

// this is here to expose vulkan functions to glfw
#define FOXY_GLFW_INCLUDE_VULKAN
#include "glfw.hpp"

namespace ifr {
  inline static void glfw_error_callback(int error, const char* message) {
    LOG(ERROR) << "GFLW: " << error << " | " << message;
  }

  Context::Context() {
    DCHECK(!instantiated_) << "Attempted second instantiation of foxy::glfw::Context";
    instantiated_ = true;

    kyt::i32 glfw_init_success = glfwInit();
    DCHECK(glfw_init_success) << "Failed to initialize GLFW!";
    kyt::i32 major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);
    glfwSetErrorCallback(glfw_error_callback);

    LOG(INFO) << "GLFW Version " << major << "." << minor << "." << revision;
  }

  Context::~Context() {
    instantiated_ = false;
    glfwTerminate();
  }
}