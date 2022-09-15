#pragma once

class GLFWwindow;

namespace foxy::vulkan {
  class Context;
}

namespace foxy::vulkan {
  class Swapchain {
  public:
    explicit Swapchain(Shared<GLFWwindow> window, Shared<Context> context);
    ~Swapchain();
  private:
    class Impl;
    Unique<Impl> pImpl_;
  };
}